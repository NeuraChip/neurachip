<?php

require '../../db.php'; // Include the DB class
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL & ~E_DEPRECATED);

$mongo = DB::getInstance();  // get instance of MongoDB client
$ui_db = $mongo->selectDatabase('UI_DB');
$collection = $ui_db->selectCollection('dataset_list');
$cursor = $collection->find();


foreach ($cursor as $document) {
    echo $document["name"] . "\n";
}

// printf("Read complete");
?>