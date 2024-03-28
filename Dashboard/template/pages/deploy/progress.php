<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL & ~E_DEPRECATED);

$experiment_name = $_GET['exp_name'];

require '../../db.php'; // Include the DB class
$mongo = DB::getInstance();  // get instance of MongoDB client
$ui_db = $mongo->selectDatabase('UI_DB');

// echo "<h1>Results for experiment: " . $experiment_name . "</h1>";

?>



<!DOCTYPE html>
<html lang="en">

<head>
  <!-- Google tag (gtag.js) -->
<!-- <script async src="https://www.googletagmanager.com/gtag/js?id=G-JHKPC8QXZ6"></script>
<script>
  window.dataLayer = window.dataLayer || [];
  function gtag(){dataLayer.push(arguments);}
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
          $description = "This is a test description";
          // Set environment variables
          putenv("LD_LIBRARY_PATH=/opt/mongo-cxx-driver/lib:/home/ktb/git/NeuraChip/DRAMsim3-1.0.0");


          // echo "<h1>" . $experiment_name . "</h1>";
          // echo "<h1>" . $description . "</h1>";
          // echo "<h1>HELLO KTB</h1>";
          $chippy = "/home/ktb/git/NeuraChip/NeuraSim/chippy.bin -n" . $experiment_name . " -d" . $description;

          // Execute in background
          $final_command = $chippy . " > /dev/null 2>/dev/null & echo $!";
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
                  <h4 class="card-title">Simulation Progress</h4>
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
                  <h4 class="card-title">Some metric</h4>
                  <p class="card-description">
                  <div id="some-metric">
                    Items
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