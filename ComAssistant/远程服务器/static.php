<?php
	//|POST||startTime:20200319164414|lastRunTime:546|lastTxCnt:269152|lastRxCnt:269152
    $runCnt = 0;
    $lastRunTime = 0;
    $lastTxCnt = 0;
    $lastRxCnt = 0;
	
	$val = "";
    $fileName = $_GET["filename"];//从URL中获取文件名称,格式http://www.inhowe.com/test.php?filename=a.txt
    @$data = fopen($fileName,'a+');//添加不覆盖，首先会判断这个文件是否存在，如果不存在，则会创建该文件
	//应该以键值对的形式提交信息
    if($_POST){
        $val.='|POST|';
        $cnt = 0;
        foreach($_POST as $key =>$value){
            $val .= '|'.$key.":".$value;
            $cnt = $cnt + 1;
            switch ($cnt) {
            	case 1:
            		// code...
            		break;
            	case 2:
            		$lastRunTime = $value;
            		break;
        		case 3:
	        		$lastTxCnt = $value;
	        		break;
        		case 4:
            		$lastRxCnt = $value;
            		break;
            	default:
            		// code...
            		break;
            }
        }
    }else{
        $val.='|GET|';
        foreach($_GET as $key =>$value){
                $val .= '|'.$key.":".$value;
        }
    }
    $val.= "\n";
    fwrite($data,$val);//写入文本中
    fclose($data);
    
    /*
    * 每日统计整合
    */
	//文件路径
	$dailyStaticPath = "DailyStatic/".date('Y-m').".txt";
	
	//先a+打开文件防止文件不存在
	$handle = fopen($dailyStaticPath,'a+');
	fclose($handle);
	
	$currentDate = date('Y-m-d');
	
	//读最后一行的数据并从文件中删除
	$resaveStr = "";
	$resaveCnt = 0;
    $handle = fopen($dailyStaticPath,'r');
    $recentStr = fgets($handle);
    while(!feof($handle)) {
		$recentStr = fgets($handle);
		//连接文本
		$resaveCnt = $resaveCnt + 1;
		if($resaveCnt > 1){
			$resaveStr .= $recentStr;
		}
		// echo $recentStr;
	}
    fclose($handle);

    // echo $recentStr."<br>";
    // echo $resaveStr."<br>";
    
    $runCnt;
    $runTime;
    $txCnt;
    $rxCnt;
    //先加回车
    $saveStr = "\n";
    //判断今天是否有数据
    if($currentDate == substr($recentStr,0,10)){
    	// echo "has record";
    	//今天有记录了
    	
  //  	//删除问题未解决
  //  	//先把最后一行删除
	 //   //重新写入文本中，这样就剔除了最后一行
		// $handle = fopen($dailyStaticPath,'w');
		// fwrite($handle,$resaveStr);
	 //   fclose($handle);
    
    	//开始匹配数据
		preg_match("/runCnt:\d+/", $recentStr, $matches);
		$runCnt = intval(substr($matches[0],7));
		$runCnt = $runCnt + 1;
		$runCnt = strval($runCnt);
		
		preg_match("/runTime:\d+/", $recentStr, $matches);
		$runTime = intval(substr($matches[0],8));
		$runTime = $runTime + intval($lastRunTime);
		$runTime = strval($runTime);

		preg_match("/txCnt:\d+/", $recentStr, $matches);
		$txCnt = intval(substr($matches[0],6));
		$txCnt = $txCnt + intval($lastTxCnt);
		$txCnt = strval($txCnt);
		
		preg_match("/rxCnt:\d+/", $recentStr, $matches);
		$rxCnt = intval(substr($matches[0],6));
		$rxCnt = $rxCnt + intval($lastRxCnt);
		$rxCnt = strval($rxCnt);
		
		$saveStr .= $currentDate. ":". " runCnt:". $runCnt. " runTime:".$runTime." txCnt:".$txCnt." rxCnt:".$rxCnt;
		
		// echo $saveStr;
    }else{
    	// echo "no record";
    	//今天没记录
    	$saveStr .= $currentDate.":"." runCnt:1"." runTime:".$lastRunTime." txCnt:".$lastTxCnt." rxCnt:".$lastRxCnt;
    }

    //从最后一行写入
    $handle = fopen($dailyStaticPath,'a+');
    fwrite($handle,$saveStr);//写入文本中
    fclose($handle);

?>