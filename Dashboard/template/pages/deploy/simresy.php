<?php
$experiment_name = $_GET['experiment_name']; // Make sure to sanitize this input

// Ensure safe filename
$experiment_name_safe = preg_replace('/[^a-zA-Z0-9-_]/', '_', $experiment_name);

$outputFile = "/tmp/" . $experiment_name_safe . "_output.txt";

if (file_exists($outputFile)) {
	$output = file_get_contents($outputFile);
	echo $output;
} else {
	echo "Output file not found.";
}
?>