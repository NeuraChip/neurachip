<?php
$latency = shell_exec('ping neurachip.kaustubh.us -w 2 -c 1 | grep "time=" | cut -d" " -f8 | cut -d"=" -f2');
echo json_encode(['latency' => $latency, 'isFinished' => false]);
?>