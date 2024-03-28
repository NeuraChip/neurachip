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
  <!-- Required meta tags -->
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
  <title>Launch NeuraSim</title>
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
    require('../../navbar.php')
      ?>

    <!-- partial -->
    <div class="container-fluid page-body-wrapper">
      <!-- partial:../../partials/_navbar.html -->
      <?php
      require('../../topbar.php')
        ?>


      <!-- partial -->

      <div class="main-panel">

        <form class="form-sample" action="sim_status.php" method="post">

          <div class="content-wrapper">

            <div class="col-12 grid-margin">
              <div class="card">
                <div class="card-body">
                  <h2 class="card-title">New <span class="text-success">Neura</span><span class="text-danger">Sim</span>
                    Simulation</h2>






                  <h3 class="card-description"> New Experiment </h3>
                  <div class="row">
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label" for="experiment_name">Experiment Name</label>
                        <div class="col-sm-9">
                          <input type="text" class="form-control" id="experiment_name" name="experiment_name" />
                        </div>
                      </div>
                    </div>
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label" for="description">Description</label>
                        <div class="col-sm-9">
                          <input type="text" class="form-control" id="description" name="description" value="Write experiment description here." />
                        </div>
                      </div>
                    </div>
                  </div>

                  <script>
                    document.addEventListener('DOMContentLoaded', function () {
                      var birdNames = [
                        "Albatross", "Bluebird", "Canary", "Dove", "Eagle", "Falcon",
                        "Goldfinch", "Hawk", "Ibis", "Jay", "Kingfisher", "Lark",
                        "Magpie", "Nightingale", "Owl", "Parrot", "Quail", "Raven",
                        "Swan", "Toucan", "Umbrellabird", "Vulture", "Woodpecker",
                        "Yellowhammer", "Zebrafinch",
                        // Add more bird names here, each as a single word
                      ];
                      var randomBird = birdNames[Math.floor(Math.random() * birdNames.length)];
                      document.getElementById('experiment_name').value = randomBird;
                    });
                  </script>


                  <div class="row">
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">Dataset</label>
                        <div class="col-sm-9">
                          <select class="form-control" id="dataset-dropdown">
                            <?php
                            // $dataset_list = $ui_db->selectCollection('dataset_list');
                            // $dataset_cursor = $dataset_list->find();
                            // foreach ($dataset_cursor as $dataset) {
                            //   echo "<option>" . $dataset['name'] . "</option>";
                            // }
                            ?>
                            <option>Plantoid Cora</option>
                          </select>
                        </div>
                      </div>
                    </div>
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">Data desc.</label>

                        <script>
                          $(document).ready(function () {
                            $('#dataset-dropdown').change(function () {
                              var selectedOption = $(this).val();
                              $.ajax({
                                url: 'dash_status/latency_update.php',
                                type: 'GET',
                                data: { option: selectedOption }, // Pass selected option to the server
                                success: function (data) {
                                  var parsed_data = JSON.parse(data);
                                  $('#ktb_latency').html(parsed_data.latency);
                                },
                                error: function (error) {
                                  console.log('Error:', error);
                                }
                              });
                            });
                          });
                        </script>



                        <div class="col-sm-9">
                          <!-- <input type="text" class="form-control" name="description" /> -->
                          <!-- Diasabled data description -->
                          <!-- <input type="text" class="form-control" name="description" disabled /> -->
                          <p class="form-control-static">
                          <div id="data_desc">
                            Nodes: 2708 Edges: 5429
                          </div>
                          </p>
                        </div>
                      </div>
                    </div>
                  </div>

                  <!-- <div class="row">
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">Gender</label>
                        <div class="col-sm-9">
                          <select class="form-control">
                            <option>Male</option>
                            <option>Female</option>
                          </select>
                        </div>
                      </div>
                    </div>
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">Date of Birth</label>
                        <div class="col-sm-9">
                          <input class="form-control" placeholder="dd/mm/yyyy" />
                        </div>
                      </div>
                    </div>
                  </div> -->

                  <div class="row">
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">Sim Interval</label>
                        <div class="col-sm-9">
                          <select class="form-control">
                            <option>Every million cycles</option>
                            <option selected>Every thousand cycles</option>
                            <option>Every hundred cycles</option>
                            <option>Every ten cycles</option>
                            <option>Every cycle</option>
                          </select>
                        </div>
                      </div>
                    </div>
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">Threads</label>
                        <div class="col-sm-4">
                          <div class="form-check">
                            <label class="form-check-label">
                              <input type="radio" class="form-check-input" name="membershipRadios"
                                id="membershipRadios1" value="" checked> Single threaded </label>
                          </div>
                        </div>
                        <div class="col-sm-5">
                          <div class="form-check">
                            <label class="form-check-label">
                              <input type="radio" class="form-check-input" name="membershipRadios"
                                id="membershipRadios2" value="option2"> Multi threaded </label>
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>

                  <h3 class="card-description"> NeuraChip Accelerator Configuration </h3>

                  <div class="row">
                    <div class="col-md-6">

                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label" for="tile-config" >Tile Config</label>
                        <div class="col-sm-9">

                          <select class="form-control" id="tile-config" name="tile-config">
                            <option selected>Tile 16</option>
                            <option>Tile 64</option>
                            <option>Tile 256</option>
                          </select>


                        </div>
                      </div>

                    </div>
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">MMH Config</label>
                        <div class="col-sm-9">
                          <select class="form-control" id="mmh_config">
                            <option>MMH 1</option>
                            <option>MMH 2</option>
                            <option selected>MMH 4</option>
                            <option>MMH 16</option>
                          </select>
                        </div>
                      </div>
                    </div>
                  </div>

                  <div class="row">
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">Hash Accumulate Config</label>
                        <div class="col-sm-9">
                          <select class="form-control" id="hacc_config" name="hacc_config">
                            <option>Barrier Based Eviction</option>
                            <option selected>Rolling Eviction</option>
                          </select>
                        </div>
                      </div>
                    </div>
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">NeuraCore Pipeline Count</label>
                        <div class="col-sm-9">
                          <select class="form-control" id="pipeline_config", name="pipeline_config">
                            <option>32</option>
                            <option>64</option>
                            <option selected>128</option>
                            <option>256</option>
                            <option>512</option>
                            <option>1024</option>
                          </select>
                        </div>
                      </div>
                    </div>
                  </div>

                  <div class="row">
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">NeuraMem Comparator Count</label>
                        <div class="col-sm-9">
                        <select class="form-control" id="comparator_config", name="comparator_config">
                            <option>8</option>
                            <option selected>16</option>
                            <option>32</option>
                            <option>64</option>
                            <option>128</option>
                            <option>256</option>
                          </select>
                        </div>
                      </div>
                    </div>
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">Cache Lines Count</label>
                        <div class="col-sm-9">
                        <select class="form-control" id="cache_line_config", name="cache_line_config">
                            <option>1024</option>
                            <option>2048</option>
                            <option selected>4096</option>
                            <option>8192</option>
                          </select>
                        </div>
                      </div>
                    </div>
                  </div>

                  <div class="row">
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">Router Buffer Size</label>
                        <div class="col-sm-9">
                        <select class="form-control" id="router_buffer_config", name="router_buffer_config">
                            <option>16</option>
                            <option selected>32</option>
                            <option>64</option>
                            <option>128</option>
                            <option>256</option>
                          </select>
                        </div>
                      </div>
                    </div>
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">HashPad Comparator Count</label>
                        <div class="col-sm-9">
                        <select class="form-control" id="comparator_count_config", name="comparator_count_config">
                            <option>4</option>
                            <option>8</option>
                            <option selected>16</option>
                            <option>32</option>
                            <option>64</option>
                          </select>
                        </div>
                      </div>
                    </div>
                  </div>

                  <!-- <div class="row">
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">HashPad Size</label>
                        <div class="col-sm-9">
                          <input type="text" class="form-control" placeholder="16384" />
                        </div>
                      </div>
                    </div>
                    <div class="col-md-6">
                      <div class="form-group row">
                        <label class="col-sm-3 col-form-label">HashPad Comparator Count</label>
                        <div class="col-sm-9">
                          <input type="text" class="form-control" placeholder="4" />
                        </div>
                      </div>
                    </div>
                  </div> -->


                </div>
              </div>
            </div>










            <div class="col-12 grid-margin">
              <div class="card">
                <div class="card-body">
                  <!-- Put launch button in center of card -->
                  <div class="text-center">
                    <a href="sim_status.php">
                      <button type="submit" class="btn btn-outline-danger btn-icon-text btn-lg">
                        <i class="mdi mdi-rocket btn-icon-prepend"></i> LAUNCH
                      </button>
                    </a>
                  </div>


                </div>
              </div>
            </div>



        </form>

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