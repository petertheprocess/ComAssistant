#include "myqcustomplot.h"

MyQCustomPlot::MyQCustomPlot(QWidget* parent)
    :QCustomPlot(parent)
{
    // connect slot that ties some axis selections together (especially opposite axes):
    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(this, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(this, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(this, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(this, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));

    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(this, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));

    // setup policy and connect slot for context menu popup:
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
    // 坐标跟随
    connect(this, SIGNAL(mouseMove(QMouseEvent*)), this,SLOT(showTracer(QMouseEvent*)));

    m_Tracer = new MyTracer(this, this->graph(), TracerType::DataTracer);

}

MyQCustomPlot::~MyQCustomPlot()
{
    delete m_Tracer;
}

void MyQCustomPlot::init(QStatusBar* pBar, QCustomPlotControl* control, DataProtocol* proto)
{
    bar = pBar;
    plotControl = control;
    protocol = proto;
}

/*plotter交互*/
void MyQCustomPlot::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
    qDebug()<<"axisLabelDoubleClick";
  // Set an axis label by double clicking on it
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, "更改轴标签", "新的轴标签：",
                                             QLineEdit::Normal, axis->label(), &ok, Qt::WindowCloseButtonHint);
    if (ok)
    {
      axis->setLabel(newLabel);
      this->replot();
    }
  }else if(part == QCPAxis::spAxis){
      if(axis==this->yAxis||axis==this->yAxis2){
         bar->showMessage("Y轴无法手动调整", 2000);
         return;
      }
      bool ok;
      double newLength = QInputDialog::getDouble(this, "更改X轴长度", "新的X轴长度：",plotControl->getXAxisLength(),
                                                 0, 10000, 1, &ok, Qt::WindowCloseButtonHint);
      if (ok)
      {
        plotControl->setXAxisLength(newLength);
        this->replot();
      }
  }
}

void MyQCustomPlot::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, "更改曲线名称", "新的曲线名称",
                                            QLineEdit::Normal, plItem->plottable()->name(), &ok, Qt::WindowCloseButtonHint);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      plotControl->getNameSetsFromPlot();
      this->replot();
    }
  }
}

void MyQCustomPlot::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      this->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    this->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    this->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      this->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    this->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    this->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<this->graphCount(); ++i)
  {
    QCPGraph *graph = this->graph(i);
    QCPPlottableLegendItem *item = this->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    }
  }
}

void MyQCustomPlot::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    this->axisRect()->setRangeDrag(this->xAxis->orientation());
  else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    this->axisRect()->setRangeDrag(this->yAxis->orientation());
  else
    this->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void MyQCustomPlot::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
        this->axisRect()->setRangeZoom(this->xAxis->orientation());
        plotControl->setXAxisLength(this->xAxis->range().upper - this->xAxis->range().lower);
    }
    else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
        this->axisRect()->setRangeZoom(this->yAxis->orientation());
    }
    else{
        //只调X轴
        this->axisRect()->setRangeZoom(Qt::Horizontal);
        plotControl->setXAxisLength(this->xAxis->range().upper - this->xAxis->range().lower);
    }
}

void MyQCustomPlot::removeSelectedGraph()
{
    QMessageBox::Button res;
    res = QMessageBox::warning(this,"警告","确定要移除所选曲线吗？",QMessageBox::Ok|QMessageBox::No);
    if(res == QMessageBox::No)
        return;

    if (this->selectedGraphs().size() > 0)
    {
        this->removeGraph(this->selectedGraphs().first());
        this->rescaleAxes(true);
        this->replot();
    }
}

void MyQCustomPlot::rescaleYAxis()
{
    this->yAxis->rescale(true);
    this->replot();
}

void MyQCustomPlot::removeAllGraphs()
{
    QMessageBox::Button res;
    res = QMessageBox::warning(this,"警告","确定要移除所有曲线吗？",QMessageBox::Ok|QMessageBox::No);
    if(res == QMessageBox::No)
        return;

    protocol->clearBuff();
    plotControl->clearPlotter(this, -1);
    while(this->graphCount()>1){
        this->removeGraph(this->graphCount()-1);
    }
    this->yAxis->setRange(0,5);
    this->xAxis->setRange(0, plotControl->getXAxisLength(), Qt::AlignRight);
    this->replot();
}

void MyQCustomPlot::hideSelectedGraph()
{
    if (this->selectedGraphs().size() > 0)
    {
        //获取图像编号
        int index = 0;
        for(;index < this->graphCount(); index++){
            if(this->graph(index)->name() == this->selectedGraphs().first()->name()){
                break;
            }
        }
        //可见性控制
        if(this->selectedGraphs().first()->visible()){
            this->selectedGraphs().first()->setVisible(false);
            this->legend->item(index)->setTextColor(Qt::gray);
        }
        else{
            this->selectedGraphs().first()->setVisible(true);
            this->legend->item(index)->setTextColor(Qt::black);
        }
        this->rescaleAxes(true);
        this->replot();
    }
}

void MyQCustomPlot::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  if (this->legend->selectTest(pos, false) >= 0) // context menu on legend requested
  {
    menu->addAction("移动到左上角", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignTop|Qt::AlignLeft));
    menu->addAction("移动到右上角", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignTop|Qt::AlignRight));
    menu->addAction("移动到右下角", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignBottom|Qt::AlignRight));
    menu->addAction("移动到左下角", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignBottom|Qt::AlignLeft));
  } else  // general context menu on graphs requested
  {
    if (this->graphCount() > 0){
      menu->addAction("曲线居中", this, SLOT(rescaleYAxis()));
      menu->addAction("移除所有曲线", this, SLOT(removeAllGraphs()));
    }
  }
  //选择了曲线
  if (this->selectedGraphs().size() > 0){
    menu->addSeparator();
    menu->addAction("移除所选曲线", this, SLOT(removeSelectedGraph()));
    menu->addSeparator();
    //所选曲线是否可见
    if(this->selectedGraphs().first()->visible()){
        menu->addAction("隐藏所选曲线", this, SLOT(hideSelectedGraph()));
    }else{
        menu->addAction("显示所选曲线", this, SLOT(hideSelectedGraph()));
    }
  }

  menu->popup(this->mapToGlobal(pos));
}

void MyQCustomPlot::moveLegend()
{
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
  {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok)
    {
      this->axisRect()->insetLayout()->setInsetAlignment(0, static_cast<Qt::Alignment>(dataInt));
      this->replot();
    }
  }
}

void MyQCustomPlot::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
  // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
  // usually it's better to first check whether interface1D() returns non-zero, and only then use it.

//  double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
//  QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
//  qDebug()<<message;
}

void MyQCustomPlot::showTracer(QMouseEvent *event)
{
    if(this->selectedGraphs().size() <= 0){
        m_Tracer->setVisible(false);
        this->replot();
        return;
    }
    m_Tracer->setVisible(true);

    //获取容器
    QSharedPointer<QCPGraphDataContainer> tmpContainer;
    tmpContainer = this->selectedGraphs().first()->data();

    //获取x轴坐标
    double x = this->xAxis->pixelToCoord(event->pos().x());
    x = static_cast<int>(x+0.5);// 四舍五入取整
    //获取Y轴坐标
    double y = 0;
    y = (tmpContainer->constBegin()+static_cast<int>(x))->mainValue();

    //范围约束
    QCPRange xRange = this->axisRect()->axis(QCPAxis::atBottom, 0)->range();
    QCPRange yRange = this->axisRect()->axis(QCPAxis::atLeft, 0)->range();
    if(x > xRange.upper)
        x = xRange.upper;
    if(x < xRange.lower)
        x = xRange.lower;
    if(y > yRange.upper)
        y = yRange.upper;
    if(y < yRange.lower)
        y = yRange.lower;

    //更新Tracer
    QString text = "X:" + QString::number(x, 'f', 2) + " Y:" + QString::number(y, 'f', 2);
    m_Tracer->updatePosition(x, y);
    m_Tracer->setText(text);

    this->replot();
}

/*
 * Function:保存图像为文本格式，可以选择不同的分隔符，csv文件可以用,
*/
bool MyQCustomPlot::saveGraphAsTxt(const QString& filePath, char separate)
{
    //列表头和值
    double value;
    QString txtBuff;
    QSharedPointer<QCPGraphDataContainer> tmpContainer;

    //构造表头
    for(int j = 0; j < this->graphCount(); j++){
        txtBuff += this->graph(j)->name() + separate;
    }
    txtBuff += "\n";

    //构造数据行
    int dataLen = this->graph(0)->data()->size();
    for(int i = 0; i < dataLen; i++){
        for(int j = 0; j < this->graphCount(); j++){
            tmpContainer = this->graph(j)->data();
            value = (tmpContainer->constBegin()+i)->mainValue();
            txtBuff += QString::number(value,'f') + separate;
        }
        txtBuff += "\n";
    }

    QFile file(filePath);
    if(!file.open(QFile::WriteOnly|QFile::Text))
        return false;
    file.write(txtBuff.toLocal8Bit());
    file.flush();
    file.close();
    return true;
}
