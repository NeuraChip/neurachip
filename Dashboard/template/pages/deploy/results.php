<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL & ~E_DEPRECATED);

$experiment_name = $_GET['exp_name'];

require '../../db.php'; // Include the DB class
$mongo = DB::getInstance();  // get instance of MongoDB client
// $ui_db = $mongo->selectDatabase('UI_DB');
// Select experiment_name database
$gc_db = $mongo->selectDatabase($experiment_name);
$gc_collection = $gc_db->selectCollection('H_GlobalCounters');
$document = $gc_collection->findOne([], ['sort' => ['_id' => -1]]);

// echo "<h1>Results for experiment: " . $experiment_name . "</h1>";

?>

<!DOCTYPE html>
<html lang="en">

<head>
  <!-- Google tag (gtag.js) -->
  <!-- <script async src="https://www.googletagmanager.com/gtag/js?id=G-JHKPC8QXZ6"></script>
  <script>
    window.dataLayer = window.dataLayer || [];
    function gtag() { dataLayer.push(arguments); }
    gtag('js', new Date());

    gtag('config', 'G-JHKPC8QXZ6');
  </script> -->


  <!-- Required meta tags -->
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
  <title>NeuraChip Admin</title>
  <!-- plugins:css -->
  <link rel="stylesheet" href="../../assets/vendors/mdi/css/materialdesignicons.min.css">
  <link rel="stylesheet" href="../../assets/vendors/css/vendor.bundle.base.css">
  <!-- endinject -->
  <!-- Plugin css for this page -->
  <!-- End Plugin css for this page -->
  <!-- inject:css -->
  <!-- endinject -->
  <!-- Layout styles -->
  <link rel="stylesheet" href="../../assets/css/style.css">
  <!-- End layout styles -->
  <link rel="shortcut icon" href="../../assets/images/favicon.png" />
  <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
</head>


<body>
  <div class="container-scroller">


    <!-- partial:../../partials/_sidebar.html -->

    <?php
    require("../../navbar.php");
    ?>



    <!-- partial -->
    <div class="container-fluid page-body-wrapper">
      <!-- partial:../../partials/_navbar.html -->


      <?php
      require("../../topbar.php");
      ?>





      <!-- partial -->
      <div class="main-panel">
        <div class="content-wrapper">
          <div class="page-header">
            <h2 class="page-title"> NeuraViz results for Experiment:
              <?php echo $experiment_name; ?>
            </h2>
            <nav aria-label="breadcrumb">
              <ol class="breadcrumb">
                <li class="breadcrumb-item"><a href="#">NeuraViz</a></li>
                <li class="breadcrumb-item active" aria-current="page">Results</li>
              </ol>
            </nav>
          </div>




          <div class="col-12 grid-margin">
            <div class="card">
              <div class="card-body">

                <!-- Put launch button in center of card -->
                <div id="sim_completed" class="text-center">
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/tile/tile.html"
                    style="border:none; width:100%; height:800px;"></iframe>
                </div>

              </div>
            </div>
          </div>




          <div class="col-lg-12 grid-margin stretch-card">
            <div class="card">
              <div class="card-body">
                <h4 class="card-title">Simulation Results</h4>
                <p class="card-description"> <code>NeuraSim performance metrics</code>
                </p>
                <div class="table-responsive">


                  <!-- Experiment Name, Tile Size, Pipeline Count, Comparator Count, Total Cycles, Simulator Wall Time, Average Simulation Speed, Total Memory Requests, Total Memory Responses, Total Hash Requests, Total Hash Responses -->
                  <!-- <tr>
                            <td> Experiment Name </td>
                            <td> Get from DB </td>
                            <td> Generate some static description </td>
                          </tr> -->
                  <?php
                  if ($document) {
                    // HTML for the table
                    echo "<table class='table table-hover'>
            <thead>
                <tr>
                    <th> <h4> Performance Metric </h4> </th>
                    <th> <h4> Value </h4> </th>
                    <th> <h4> Description </h4> </th>
                </tr>
            </thead>
            <tbody>";

                    // Function to output a table row
                    function outputRow($metric, $value, $description)
                    {
                      echo "<tr>
                <td class=\"text-light\"> $metric </td>
                <td> $value </td>
                <td class=\"text-light\"> $description </td>
              </tr>";
                    }

                    // Output rows for each metric
                    outputRow('Experiment Name', '<label class="btn btn-inverse-info btn-fw">' . $document['exp_name'] . '</label>', 'Name of the experiment');
                    outputRow('Compute Units', '<label class="btn btn-inverse-info btn-fw">' . "2 x " . $document['tile_size'] . '</label>', 'Number of NeuraCores and NeuraMems in each row and column of the mesh');
                    outputRow('Frequency', '<label class="btn btn-inverse-info btn-fw">' . "1 GHz" . '</label>', 'Operating frequency of the NeuraChip');
                    outputRow('Peak Performance', '<label class="btn btn-inverse-info btn-fw">' . $document['peak_performance'] . " GFLOPs"  . '</label>', 'Peak FLOPs');

                    outputRow('SpGEMM Performance', '<label class="btn btn-inverse-success btn-fw">' . $document['spgemm_performance'] . " GOP/s" . '</label>', 'Sparse Matrix Multiplication average performance');
                    outputRow('On Chip Memory', '<label class="btn btn-inverse-info btn-fw">' . $document['on_chip_memory'] . '</label>', 'On chip memory size');
                    outputRow('Off Chip Memory', '<label class="btn btn-inverse-info btn-fw">' . $document['off_chip_memory'] . " GB" . '</label>', 'HBM size');
                    outputRow('Technology', '<label class="btn btn-inverse-info btn-fw">' . $document['technology'] . '</label>', 'Synthesizing technology');
                    outputRow('Chip Area', '<label class="btn btn-inverse-info btn-fw">' . $document['chip_area'] . '</label>', 'Total chip area of NeuraChip');
                    outputRow('Area Efficiency', '<label class="btn btn-inverse-info btn-fw">' . $document['area_efficiency'] . '</label>', 'Area efficiency of NeuraChip accelerator');
                    
                    


                    outputRow('Pipeline Count', '<label class="btn btn-inverse-info btn-fw">' . $document['pipeline_count'] . '</label>', 'Number of pipelines used in each NeuraCore. Increasing pipeline count will increase the register file size for each NeuraCore');
                    outputRow('Comparator Count', '<label class="btn btn-inverse-info btn-fw">' . $document['hashpad_comparator_count'] . '</label>', 'Number of hashpad comparators used in each NeuraMem. Increasing comparator count will improve simultaneous <span class=text-info>TAG</span> comparisons in HashPad memory');
                    outputRow('Total Cycles', '<label class="btn btn-inverse-success btn-fw">' . $document['cycles'] . '</label>', 'Total number of cycles in the experiment');
                    outputRow('Execution Time', '<label class="btn btn-inverse-success btn-fw">' . $document['cycles'] / 1000 . ' microseconds </label>', 'Time taken by accelerator to complete the workload');
                    outputRow('Simulator Wall Time', '<label class="btn btn-inverse-success btn-fw">' . $document['sim_time'] . '</label>', 'Total wall time required to complete the simulation');
                    outputRow('Average Simulation Speed (MCPS)', '<label class="btn btn-inverse-warning btn-fw">' . $document['cps'] . '</label>', 'Mean cycles per second in the simulation');
                    outputRow('Total Memory Requests', '<label class="btn btn-inverse-danger btn-fw">' . $document['total_mem_reqs'] . '</label>', 'Total memory requests made');
                    outputRow('Total Memory Responses', '<label class="btn btn-inverse-danger btn-fw">' . $document['total_mem_resps'] . '</label>', 'Total memory responses received');
                    outputRow('Total Hash Requests', '<label class="btn btn-inverse-danger btn-fw">' . $document['total_hash_reqs'] . '</label>', 'Total hash requests made');
                    outputRow('Total Hash Responses', '<label class="btn btn-inverse-danger btn-fw">' . $document['total_hash_resps'] . '</label>', 'Total hash responses received');

                    echo "</tbody>
        </table>";
                  } else {
                    echo "<p>No data found for the specified experiment.</p>";
                  }
                  ?>



                </div>
              </div>
            </div>
          </div>





          <div class="row">



            <!-- <div class="col-lg-6 grid-margin stretch-card">
                <div class="card">
                  <div class="card-body">
                    <h4 class="card-title">Line chart</h4>
                    <canvas id="lineChart" style="height:250px"></canvas>
                  </div>
                </div>
              </div> -->

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    NeuraCore Busy Cycles
                  </h4>
                  <p> Percentage of total cycles that the NeuraCore spent computing (Not Stalled). </p>
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/NeuraCore_busy_cycles.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>

                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/NeuraCore_busy_cycles.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>



            <!-- <div class="col-lg-6 grid-margin stretch-card">
                <div class="card">
                  <div class="card-body">
                    <h4 class="card-title">Bar charty</h4>
                    <canvas id="barChart" style="height:230px"></canvas>
                  </div>
                </div>
              </div> -->

            <!-- <div class="col-lg-6 grid-margin stretch-card">
                <div class="card">
                  <div class="card-body">
                    <h4 class="card-title">Bar chart</h4>
                    <div id="plotlyBarChart" style="height:230px;"></div>
                    <script>
                      var trace1 = {
                        x: ['A', 'B', 'C'],
                        y: [10, 15, 7],
                        type: 'bar'
                      };
              
                      var data = [trace1];
              
                      var layout = {
                        title: 'Sample Bar Chart',
                      };
              
                      Plotly.newPlot('plotlyBarChart', data, layout);
                    </script>
                  </div>
                </div>
              </div> -->


            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    NeuraMem Busy Cycles
                  </h4>
                  <p> Percentage of total cycles that the NeuraMem spent computing (Not Stalled).
                    This includes <span class=text-info>TAG</span> comparisons in HashPad memory as well as <span
                      class=text-info>DATA</span> accumulations. </p>
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/NeuraMem_busy_cycles.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>

                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/NeuraMem_busy_cycles.html"
                    style="border:none; width:100%; height:500px;"></iframe>

                </div>
              </div>
            </div>



          </div>



          <div class="row">



            <!-- <div class="col-lg-6 grid-margin stretch-card">
    <div class="card">
      <div class="card-body">
        <h4 class="card-title">Line chart</h4>
        <canvas id="lineChart" style="height:250px"></canvas>
      </div>
    </div>
  </div> -->

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    Register File Cycles Spent Histogram
                  </h4>
                  <p> A <span class="text-danger">higher</span> value indicates instructions are not being computed in
                    NeuraCore as they await accumulation in NeuraMem units</p>
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/neuracore/rf_histogram.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>
                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/neuracore/rf_histogram.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>



            <!-- <div class="col-lg-6 grid-margin stretch-card">
    <div class="card">
      <div class="card-body">
        <h4 class="card-title">Bar charty</h4>
        <canvas id="barChart" style="height:230px"></canvas>
      </div>
    </div>
  </div> -->

            <!-- <div class="col-lg-6 grid-margin stretch-card">
    <div class="card">
      <div class="card-body">
        <h4 class="card-title">Bar chart</h4>
        <div id="plotlyBarChart" style="height:230px;"></div>
        <script>
          var trace1 = {
            x: ['A', 'B', 'C'],
            y: [10, 15, 7],
            type: 'bar'
          };
  
          var data = [trace1];
  
          var layout = {
            title: 'Sample Bar Chart',
          };
  
          Plotly.newPlot('plotlyBarChart', data, layout);
        </script>
      </div>
    </div>
  </div> -->


            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    NeuraRouter Busy Cycles
                  </h4>
                  <p> Percentage of total cycles that the NeuraRouter spent computing
                    (Transferring data from one <span class="text-info">port</span> to another).
                  </p>
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>
                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>



          </div>



          <div class="row">


            <!-- <div class="col-lg-6 grid-margin stretch-card">
                <div class="card">
                  <div class="card-body">
                    <h4 class="card-title">Area chart</h4>
                    <canvas id="areaChart" style="height:250px"></canvas>
                  </div>
                </div>
              </div> -->

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    Memory Controller Access Profile
                  </h4>
                  <p> Y axis represents the memory address of read and write
                    transaction, X axis is the cycle number at which the transaction was made.
                  </p>
                  <p> Yellow = Write transactions, Blue = Read transactions.
                  </p>

                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/mc_access_profile.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>
                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/mc_access_profile.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>


            <!-- <div class="col-lg-6 grid-margin stretch-card">
                <div class="card">
                  <div class="card-body">
                    <h4 class="card-title">Doughnut chart</h4>
                    <canvas id="doughnutChart" style="height:250px"></canvas>
                  </div>
                </div>
              </div> -->
            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    Connections Busy Cycles
                  </h4>
                  Percentage of total cycles that the connection <span class="text-danger">(link between
                    NeuraCore/NeuraMem and NeuraRouter)</span> spent computing (Transferring data from one Core to
                  Router or Memory to Router).
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/Conn_busy_cycles.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>
                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/Conn_busy_cycles.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>


          </div>
          <div class="row">


            <!-- <div class="col-lg-6 grid-margin stretch-card">
                <div class="card">
                  <div class="card-body">
                    <h4 class="card-title">Pie chart</h4>
                    <canvas id="pieChart" style="height:250px"></canvas>
                  </div>
                </div>
              </div> -->

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    Writer Busy Cycles
                  </h4>
                  <p>
                    Percentage of total cycles that the Writer spent computing (Transferring data from NeuraMem HashPads
                    to <span class="text-warning">DRAM memory banks</span>).
                  </p>
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/Writer_busy_cycles.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>
                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/Writer_busy_cycles.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>


            <!-- <div class="col-lg-6 grid-margin stretch-card">
                <div class="card">
                  <div class="card-body">
                    <h4 class="card-title">Scatter chart</h4>
                    <canvas id="scatterChart" style="height:250px"></canvas>
                  </div>
                </div>
              </div> -->

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    Memory Controller Stall Profile
                  </h4>
                  <p>
                    Total cycles that the Memory Controller Channels and Writers were <span
                      class="text-danger">stalled</span>, awaiting a <span class="text-info">READ</span> or <span
                      class="text-info">WRITE</span> memory transaction.
                  </p>
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/mc_stalls.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>
                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_overall/mc_stalls.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>


          </div>




          <div class="row">

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    Writer Iterative Profile (Writer Pressure)
                  </h4>
                  <p>
                    This is a continuous interval plot, where X axis represents the cycle number and Y axis represents
                    the percentage of cycles the Writer was busy. Over 100% value indicates the writer buffers are
                    overflowing.
                  </p>
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_iterative/Writer_iterative_busy_percentage.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>
                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_iterative/Writer_iterative_busy_percentage.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    Average In Flight Memory Transactions per Cycle (Memory Pressure)
                  </h4>
                  <p>
                    This is a continuous interval plot, where X axis represents the cycle number and Y axis represents
                    the average number of memory transactions in flight. This metric is a good indicator of memory
                    pressure.
                  </p>
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/mc/avg_in_flight_requests.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>
                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/mc/avg_in_flight_requests.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>


          </div>


          <div class="row">

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    NeuraCore Iterative Profile (Individual NeuraCore Pressure)
                  </h4>
                  <p>
                    This is a continuous interval plot, where X axis represents the cycle number and Y axis represents
                    the percentage of cycles the NeuraCore was busy. Over 100% value indicates the NeuraCore buffers are
                    overflowing.
                  </p>
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_iterative/NeuraCore_iterative_busy_percentage.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>
                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_iterative/NeuraCore_iterative_busy_percentage.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    NeuraMem Iterative Profile (Individual NeuraMem Pressure)
                  </h4>
                  <p>
                    This is a continuous interval plot, where X axis represents the cycle number and Y axis represents
                    the percentage of cycles the NeuraMem was busy. Over 100% value indicates the NeuraMem buffers are
                    overflowing.
                  </p>
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_iterative/NeuraMem_iterative_busy_percentage.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>
                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_iterative/NeuraMem_iterative_busy_percentage.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>

          </div>

          <div class="row">

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    NeuraRouter Iterative Profile (Individual NeuraRouter Pressure)
                  </h4>
                  <p>
                    This is a continuous interval plot, where X axis represents the cycle number and Y axis represents
                    the percentage of cycles the NeuraRouter was busy. Over 100% value indicates the NeuraRouter buffers
                    are overflowing.
                  </p>
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_iterative/NeuraRouter_iterative_busy_percentage.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>
                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_iterative/NeuraRouter_iterative_busy_percentage.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">
                    Memory Controller Iterative Profile (Individual Memory Controller Pressure)
                  </h4>
                  <p>
                    This is a continuous interval plot, where X axis represents the cycle number and Y axis represents
                    the percentage of cycles the Memory Controller was busy. Over 100% value indicates the Memory
                    Controller buffers are overflowing.
                  </p>
                  <h4 class="card-title">
                    <a href="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_iterative/MemoryController_iterative_busy_percentage.html"
                      target="_blank">
                      Click here to enlarge plot
                    </a>
                  </h4>
                  <!-- <iframe src="https://neurachip.us/results_neurachip/Viz_V03/figs/stall_profiles_overall/NeuraRouter_busy_cycles.html" style="border:none; width:100%; height:500px;"></iframe> -->
                  <!-- Use iframe but instead of Viz_V03 use $experiment_name -->
                  <iframe
                    src="https://neurachip.us/results_neurachip/<?php echo $experiment_name; ?>/figs/stall_profiles_iterative/MemoryController_iterative_busy_percentage.html"
                    style="border:none; width:100%; height:500px;"></iframe>
                </div>
              </div>
            </div>

          </div>




        </div>
        <!-- content-wrapper ends -->
        <!-- partial:../../partials/_footer.html -->
        <footer class="footer">
          <div class="d-sm-flex justify-content-center justify-content-sm-between">
            <span class="text-muted d-block text-center text-sm-left d-sm-inline-block">Made with <i
                class="mdi mdi-heart text-danger"></i> by Kaustubh Shivdikar </span>

            <span class="float-none float-sm-right d-block mt-1 mt-sm-0 text-center">
              <i class="mdi mdi-flask-outline text-danger"></i>
              Patent Pending</span>
          </div>
        </footer>
        <!-- partial -->
      </div>
      <!-- main-panel ends -->
    </div>
    <!-- page-body-wrapper ends -->
  </div>
  <!-- container-scroller -->
  <!-- plugins:js -->
  <script src="../../assets/vendors/js/vendor.bundle.base.js"></script>
  <!-- endinject -->
  <!-- Plugin js for this page -->
  <script src="../../assets/vendors/chart.js/Chart.min.js"></script>
  <!-- End plugin js for this page -->
  <!-- inject:js -->
  <script src="../../assets/js/off-canvas.js"></script>
  <script src="../../assets/js/hoverable-collapse.js"></script>
  <script src="../../assets/js/misc.js"></script>
  <script src="../../assets/js/settings.js"></script>
  <script src="../../assets/js/todolist.js"></script>
  <!-- endinject -->
  <!-- Custom js for this page -->
  <script src="../../assets/js/chart.js"></script>
  <!-- End custom js for this page -->
</body>

</html>