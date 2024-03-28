<?php

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL & ~E_DEPRECATED);


require '../vendor/autoload.php';

// echo "KTB - Mongo Status";




try {
$mongo = new \MongoDB\Client('mongodb://localhost:27017');
$dbs = $mongo->listDatabases();
$mongo_live = 1;
// echo "KTB - Mongo Status - 1";
} catch (Exception $e) {
echo $e->getMessage();
// echo "KTB - Mongo Status - 0";
$mongo_live = 0;
}
if ($mongo_live == 1) {
	echo json_encode(['mongo_live' => $mongo_live, 'status' => 'Online', 'server_icon' => '<i class="icon-lg mdi mdi-server text-success ms-auto"></i>']);
} else {
	echo json_encode(['mongo_live' => $mongo_live, 'status' => 'Offline', 'server_icon' => '<i class="icon-lg mdi mdi-server-off text-danger ms-auto"></i>']);
}
?>