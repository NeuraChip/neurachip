<?php

require '../../db.php'; // Include the DB class
$mongo = DB::getInstance();  // get instance of MongoDB client
// $ui_db = $mongo->selectDatabase('UI_DB');
$experiment_name = $_POST['experiment_name'];

$exp_db = $mongo->selectDatabase($experiment_name);
$gc_collection = $exp_db->selectCollection('GlobalCounters');

// Get the latest document from the GlobalCounters collection
$gc_doc = $gc_collection->findOne([], ['sort' => ['_id' => -1]]);

$sim_speed = $gc_doc['interval_cps'];
// Round to closest integer
$sim_speed = round($sim_speed);

// Get interval_sim_progress from the document
$interval_sim_progress = $gc_doc['interval_sim_progress'];


$ui_db = $mongo->selectDatabase('UI_DB');
$ui_collection = $ui_db->selectCollection('UI_COLLECTION');
$ui_doc = $ui_collection->findOne(['exp_name' => $experiment_name]);
$completed = $ui_doc['completed'];

if ($completed == 1) {
    // $completed_text = "<div class='badge badge-outline-success'>Completed</div>";
    $completed_text = '<a href="https://neurachip.us/pages/deploy/results.php?exp_name='. $experiment_name . '"> <button type="button" class="btn btn-inverse-success btn-icon-text btn-lg"> <i class="mdi mdi-rocket btn-icon-prepend"></i> Results';
} else {
    $completed_text = "<div class='badge badge-outline-warning'>Pending (Can take up to 5 mins. Please Wait...)</div>";
}


$range_return_text = '<div class="range" style="--p:' . $interval_sim_progress . '"> <div class="range__label"></div> </div>';

echo json_encode(	['range_return_text' => $range_return_text,
                    'progress' => $interval_sim_progress,
					'completed_text' => $completed_text,
                    'sim_speed' => $sim_speed]);
