<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL & ~E_DEPRECATED);

require '../../db.php'; // Include the DB class

$mongo = DB::getInstance(); // get instance of MongoDB client
$ui_db = $mongo->selectDatabase('UI_DB');

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
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
  <!-- Required meta tags -->
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
  <title>Status NeuraSim</title>
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




          <?php
          $experiment_name = $_POST['experiment_name'];
          $description = $_POST['description'];
          $tile_configuration = $_POST['tile-config'];
          $pipe_line_config = $_POST['pipeline_config'];
          $comparator_configuration = $_POST['comparator_config'];
          $cache_line_configuration = $_POST['cache_line_config'];
          // Set environment variables
          putenv("LD_LIBRARY_PATH=/opt/mongo-cxx-driver/lib:/home/ktb/git/neurachip_deploy/DRAMsim3-1.0.0");


          // Ensure safe filename (consider additional sanitization as needed)
          $experiment_name_safe = preg_replace('/[^a-zA-Z0-9-_]/', '_', $experiment_name);

          // Set the output file path using the experiment name
          $outputFile = "/tmp/" . $experiment_name_safe . "_output.txt";

          // Delete the output file if it exists
          if (file_exists($outputFile)) {
            unlink($outputFile);
          }

          // Set up the command, add double inverted commas to each argument
          // $chippy = "/home/ktb/git/neurachip_deploy/NeuraSim/chippy.bin -n" . $experiment_name . " -d" . $description . " -t" . $tile_configuration;
          $chippy = "/home/ktb/git/neurachip_deploy/NeuraSim/chippy.bin -n \"" . $experiment_name . "\" -d \"" . $description . "\" -t \"" . $tile_configuration . "\" -p \"" . $pipe_line_config . "\" -m \"" . $comparator_configuration . "\" -c \"" . $cache_line_configuration . "\"";

          // Redirect output to the dynamically named file
          $final_command = $chippy . " > " . $outputFile . " 2>&1 & echo $!";

          // Echo the command to the console
          echo "<script>console.log('Command: " . $final_command . "');</script>";

          shell_exec($final_command);

          // echo "<h1>" . $experiment_name . "</h1>";
          // echo "<h1>" . $description . "</h1>";
          // echo "<h1>HELLO KTB</h1>";
          // $chippy = "/home/ktb/git/neurachip_deploy/NeuraSim/chippy.bin -n" . $experiment_name . " -d" . $description . " -t" . $tile_configuration;
          
          // Execute in background
          // $final_command = $chippy . " > /dev/null 2>/dev/null & echo $!";
          // $final_command = $chippy;
          

          // $chippy = "whoami";
          

          // Execute shell command
          // shell_exec($final_command);
          
          // $output = shell_exec($final_command);
          // echo "<pre>$output</pre>";
          // Print to console
          // echo "<script>console.log('Output: " . $output . "');</script>";
          ?>

          <!-- Create two column cards -->
          <div class="row">
            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">Experiment Name</h4>
                  <p class="card-description">
                    <?php echo $experiment_name; ?>
                  </p>
                </div>
              </div>
            </div>

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">Simulation Speed</h4>
                  <p class="card-description">
                  <div id="sim_speed">
                    0.0 cycles per second
                  </div>

                  </p>
                </div>
              </div>
            </div>
          </div>


          <!-- Write Simulation Progress in the center -->

          <script>
            $(document).ready(function () {
              setInterval(function () {
                $.ajax({
                  url: 'get_sim_progress.php',
                  type: 'POST',
                  data: {
                    'experiment_name': '<?php echo $experiment_name; ?>'
                  },
                  success: function (data) {
                    var parsed_data = JSON.parse(data);
                    // $('#glitchy-range').html(parsed_data.range_return_text);
                    $('#sim_speed').html(parsed_data.sim_speed + " cycles per second");
                    $('#sim_completed').html(parsed_data.completed_text);
                    // $completed = parsed_data.completed;
                    $('.range').css('--p', parsed_data.progress);
                    console.log(parsed_data.range_return_text);

                  },
                  error: function (error) {
                    console.log('Error:', error);
                  }
                });
              }, 1000); // Every 1 seconds
            });
          </script>

          <!-- Exp Output -->
          <script>
            function fetchOutput() {
              var xhr = new XMLHttpRequest();
              xhr.onreadystatechange = function () {
                if (xhr.readyState == 4 && xhr.status == 200) {
                  console.log(xhr.responseText);
                }
              };

              // Get the experiment name from PHP
              var experimentName = '<?php echo $experiment_name; ?>'; // Use PHP to set the experiment name

              xhr.open("GET", "simresy.php?experiment_name=" + encodeURIComponent(experimentName), true);
              xhr.send();
            }

            // Poll every 2 seconds
            setInterval(fetchOutput, 2000);
          </script>

          <script>

            // function convertAnsiToHtml(text) {
            //   // ANSI color code regex
            //   const ansiRegex = /\033\[(\d+;)?(\d+)?m/g;

            //   // Replace ANSI codes with span tags
            //   let html = text.replace(ansiRegex, (match, p1, p2) => {
            //     let colorCode = p2 ? parseInt(p2) : parseInt(p1);
            //     switch (colorCode) {
            //       case 31: return '<span style="color:red;">'; // Red
            //       case 32: return '<span style="color:green;">'; // Green
            //       case 33: return '<span style="color:yellow;">'; // Yellow
            //       // Add more colors as needed
            //       default: return '<span>';
            //     }
            //   });

            //   // Replace the reset code (0) with a closing span tag
            //   html = html.replace(/\033\[0m/g, '</span>');

            //   // Replace new lines with <br> tags
            //   html = html.replace(/\n/g, '<br>');

            //   return html;
            // }

            function convertAnsiToHtml(text) {
              // ANSI escape sequences regex
              const ansiRegex = /\x1b\[(\d+;)?(\d+)?(;)?(\d+)?m/g;

              // Replace ANSI codes with span tags
              let html = text.replace(ansiRegex, (match, p1, p2, p3, p4) => {
                // Check for combined sequences (e.g., color + underline)
                if (p1 && p2 && p4) {
                  let combinedStyles = '';
                  [p1, p2, p4].forEach(code => {
                    combinedStyles += convertAnsiCodeToStyle(parseInt(code));
                  });
                  return '<span style="' + combinedStyles + '">';
                }

                // Individual style
                let styleCode = p2 ? parseInt(p2) : parseInt(p1);
                return '<span style="' + convertAnsiCodeToStyle(styleCode) + '">';
              });

              // Replace the reset code with a closing span tag
              html = html.replace(/\x1b\[0m/g, '</span>');

              // Replace new lines with <br> tags
              html = html.replace(/\n/g, '<br>');

              return html;
            }

            // Function to convert ANSI code to CSS style
            function convertAnsiCodeToStyle(code) {
              switch (code) {
                case 31: return 'color:red;'; // Red
                case 32: return 'color:green;'; // Green
                case 33: return 'color:yellow;'; // Yellow
                case 36: return 'color:cyan;'; // Cyan
                case 37: return 'color:white;'; // White
                case 4: return 'text-decoration:underline;'; // Underline
                // Add more codes as needed
                default: return '';
              }
            }


            function fetchOutput() {
              var xhr = new XMLHttpRequest();
              xhr.onreadystatechange = function () {
                if (xhr.readyState == 4 && xhr.status == 200) {
                  var formattedOutput = convertAnsiToHtml(xhr.responseText);
                  document.getElementById('simulationOutput').innerHTML = formattedOutput;
                }
              };

              var experimentName = '<?php echo $experiment_name; ?>';
              xhr.open("GET", "simresy.php?experiment_name=" + encodeURIComponent(experimentName), true);
              xhr.send();
            }
            setInterval(fetchOutput, 2000);
          </script>






          <!-- <div class="text-center">
                  <h4 class="display-4">Simulation Progress</h4>
                </div> -->



          <!-- <div class="range text-center" style="--p:0">
                  <div class="range__label"></div>
                </div> -->




          <!-- Create two column cards -->
          <div class="row">
            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">Simulation Dispatched Instructions</h4>
                  <p class="card-description">

                  <div class="range text-center" style="--p:0">
                    <div class="range__label"></div>
                  </div>
                  </p>
                </div>
              </div>
            </div>

            <div class="col-lg-6 grid-margin stretch-card">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title">Simulation metrics</h4>
                  <p class="card-description">
                  <div id="some-metric">
                    Generating Cycle Accurate Metrics
                  </div>

                  </p>
                </div>
              </div>
            </div>
          </div>


          <div class="col-12 grid-margin">
            <div class="card">
              <div class="card-body">

                <!-- Put launch button in center of card -->
                <div id="sim_completed" class="text-center">
                  <div class='badge badge-outline-warning'>Pending</div>
                  <!-- <button type="button" class="btn btn-outline-danger btn-icon-text btn-lg">
                    <i class="mdi mdi-rocket btn-icon-prepend"></i> Results -->
                  </button>
                </div>

              </div>
            </div>
          </div>






          <?php
          require('../../server_status_row.php')
            ?>

          <?php
          require('../../server_usage_row.php')
            ?>

          <div class="col-12 grid-margin stretch-card">
            <div class="card">
              <div class="card-body" id="simulationOutput">
                <h4 class="card-title">Simulation Output</h4>
                <!-- The simulation output will be displayed here -->
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
  <!-- End plugin js for this page -->
  <!-- inject:js -->
  <script src="../../assets/js/off-canvas.js"></script>
  <script src="../../assets/js/hoverable-collapse.js"></script>
  <script src="../../assets/js/misc.js"></script>
  <script src="../../assets/js/settings.js"></script>
  <script src="../../assets/js/todolist.js"></script>
  <!-- endinject -->
  <!-- Custom js for this page -->
  <!-- End custom js for this page -->
</body>

</html>