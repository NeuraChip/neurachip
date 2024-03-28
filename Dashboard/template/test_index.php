<?php 
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);
require 'vendor/autoload.php';
?>


<?php

try {
	$mongo = new \MongoDB\Client('mongodb://localhost:27017');
	// Get the UI_DB database
	$ui_db = $mongo->selectDatabase('UI_DB');
	// Get the UI_COLLECTION collection
	$ui_collection = $ui_db->selectCollection('UI_COLLECTION');
	// Get total number of documents in the collection
	$ui_collection_count = $ui_collection->count();
	// Print the number of documents in the collection
	echo $ui_collection_count;


} catch (Exception $e) {
	// echo $e->getMessage();
}
// $dbs = $mongo->listDatabases();
// echo '<pre>';
// print_r($dbs);
// echo '</pre>';


?>

<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>NC Testing</title>
</head>
<body>
	<!-- Iterate over and print documents of UI_COLLECTION -->
	<?php
		$cursor = $ui_collection->find();
		foreach ($cursor as $document) {
			echo '<pre>';
			print_r($document['exp_name']);
			echo '</pre>';
		}
	?>
	<h1>Test</h1>
</body>
</html>



