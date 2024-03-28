<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL & ~E_DEPRECATED);

require 'db.php'; // Include the DB class

$mongo = DB::getInstance(); // get instance of MongoDB client

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
  <title>NeuraChip Console</title>
  <!-- plugins:css -->
  <link rel="stylesheet" href="assets/vendors/mdi/css/materialdesignicons.min.css">
  <link rel="stylesheet" href="assets/vendors/css/vendor.bundle.base.css">
  <!-- endinject -->
  <!-- Plugin css for this page -->
  <link rel="stylesheet" href="assets/vendors/jvectormap/jquery-jvectormap.css">
  <link rel="stylesheet" href="assets/vendors/flag-icon-css/css/flag-icon.min.css">
  <link rel="stylesheet" href="assets/vendors/owl-carousel-2/owl.carousel.min.css">
  <link rel="stylesheet" href="assets/vendors/owl-carousel-2/owl.theme.default.min.css">
  <!-- End plugin css for this page -->
  <!-- inject:css -->
  <!-- endinject -->
  <!-- Layout styles -->
  <link rel="stylesheet" href="assets/css/style.css">
  <!-- End layout styles -->
  <link rel="shortcut icon" href="assets/images/favicon.png" />
</head>




<body>
  <div class="container-scroller">
    <!-- <div class="row p-0 m-0 proBanner" id="proBanner">
        <div class="col-md-12 p-0 m-0">
          <div class="card-body card-body-padding d-flex align-items-center justify-content-between">
            <div class="ps-lg-1">
              <div class="d-flex align-items-center justify-content-between">
                <p class="mb-0 font-weight-medium me-3 buy-now-text">Free 24/7 customer support, updates, and more with this template!</p>
                <a href="https://www.bootstrapdash.com/product/corona-free/?utm_source=organic&utm_medium=banner&utm_campaign=buynow_demo" target="_blank" class="btn me-2 buy-now-btn border-0">Get Pro</a>
              </div>
            </div>
            <div class="d-flex align-items-center justify-content-between">
              <a href="https://www.bootstrapdash.com/product/corona-free/"><i class="mdi mdi-home me-3 text-white"></i></a>
              <button id="bannerClose" class="btn border-0 p-0">
                <i class="mdi mdi-close text-white me-0"></i>
              </button>
            </div>
          </div>
        </div>
      </div> -->
    <!-- partial:partials/_sidebar.html -->

    <?php
    require('./navbar.php')
      ?>



    <!-- partial -->
    <div class="container-fluid page-body-wrapper">

      <!-- partial:partials/_navbar.html -->
      <?php
      require('./topbar.php')
        ?>







      <!-- partial -->
      <div class="main-panel">
        <div class="content-wrapper">
          <!-- <div class="row">
              <div class="col-12 grid-margin stretch-card">
                <div class="card corona-gradient-card">
                  <div class="card-body py-0 px-0 px-sm-3">
                    <div class="row align-items-center">
                      <div class="col-4 col-sm-3 col-xl-2">
                        <img src="assets/images/dashboard/Group126@2x.png" class="gradient-corona-img img-fluid" alt="">
                      </div>
                      <div class="col-5 col-sm-7 col-xl-8 p-0">
                        <h4 class="mb-1 mb-sm-0">Want even more features?</h4>
                        <p class="mb-0 font-weight-normal d-none d-sm-block">Check out our Pro version with 5 unique layouts!</p>
                      </div>
                      <div class="col-3 col-sm-2 col-xl-2 ps-0 text-center">
                        <span>
                          <a href="https://www.bootstrapdash.com/product/corona-admin-template/" target="_blank" class="btn btn-outline-light btn-rounded get-started-btn">Upgrade to PRO</a>
                        </span>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div> -->





          <?php
          require('./server_status_row.php')
            ?>

          <?php
          require('./server_usage_row.php')
            ?>

          <div class="col-12 grid-margin">
            <div class="card">
              <div class="card-body">
                <!-- Put launch button in center of card -->
                <div class="text-center">
                  <a href="https://neurachip.us/pages/deploy/new_sim_form.php">
                    <button type="submit" class="btn btn-outline-danger btn-icon-text btn-lg">
                      <i class="mdi mdi-rocket btn-icon-prepend"></i> LAUNCH A NEW SIMULATION
                    </button>
                  </a>
                </div>


              </div>
            </div>
          </div>

          <!-- <div class="row">
              <div class="col-md-4 grid-margin stretch-card">
                <div class="card">
                  <div class="card-body">
                    <h4 class="card-title">Transaction History</h4>
                    <canvas id="transaction-history" class="transaction-chart"></canvas>
                    <div class="bg-gray-dark d-flex d-md-block d-xl-flex flex-row py-3 px-4 px-md-3 px-xl-4 rounded mt-3">
                      <div class="text-md-center text-xl-left">
                        <h6 class="mb-1">Transfer to Paypal</h6>
                        <p class="text-muted mb-0">07 Jan 2019, 09:12AM</p>
                      </div>
                      <div class="align-self-center flex-grow text-right text-md-center text-xl-right py-md-2 py-xl-0">
                        <h6 class="font-weight-bold mb-0">$236</h6>
                      </div>
                    </div>
                    <div class="bg-gray-dark d-flex d-md-block d-xl-flex flex-row py-3 px-4 px-md-3 px-xl-4 rounded mt-3">
                      <div class="text-md-center text-xl-left">
                        <h6 class="mb-1">Tranfer to Stripe</h6>
                        <p class="text-muted mb-0">07 Jan 2019, 09:12AM</p>
                      </div>
                      <div class="align-self-center flex-grow text-right text-md-center text-xl-right py-md-2 py-xl-0">
                        <h6 class="font-weight-bold mb-0">$593</h6>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
              <div class="col-md-8 grid-margin stretch-card">
                <div class="card">
                  <div class="card-body">
                    <div class="d-flex flex-row justify-content-between">
                      <h4 class="card-title mb-1">Open Projects</h4>
                      <p class="text-muted mb-1">Your data status</p>
                    </div>
                    <div class="row">
                      <div class="col-12">
                        <div class="preview-list">
                          <div class="preview-item border-bottom">
                            <div class="preview-thumbnail">
                              <div class="preview-icon bg-primary">
                                <i class="mdi mdi-file-document"></i>
                              </div>
                            </div>
                            <div class="preview-item-content d-sm-flex flex-grow">
                              <div class="flex-grow">
                                <h6 class="preview-subject">Admin dashboard design</h6>
                                <p class="text-muted mb-0">Broadcast web app mockup</p>
                              </div>
                              <div class="me-auto text-sm-right pt-2 pt-sm-0">
                                <p class="text-muted">15 minutes ago</p>
                                <p class="text-muted mb-0">30 tasks, 5 issues </p>
                              </div>
                            </div>
                          </div>
                          <div class="preview-item border-bottom">
                            <div class="preview-thumbnail">
                              <div class="preview-icon bg-success">
                                <i class="mdi mdi-cloud-download"></i>
                              </div>
                            </div>
                            <div class="preview-item-content d-sm-flex flex-grow">
                              <div class="flex-grow">
                                <h6 class="preview-subject">Wordpress Development</h6>
                                <p class="text-muted mb-0">Upload new design</p>
                              </div>
                              <div class="me-auto text-sm-right pt-2 pt-sm-0">
                                <p class="text-muted">1 hour ago</p>
                                <p class="text-muted mb-0">23 tasks, 5 issues </p>
                              </div>
                            </div>
                          </div>
                          <div class="preview-item border-bottom">
                            <div class="preview-thumbnail">
                              <div class="preview-icon bg-info">
                                <i class="mdi mdi-clock"></i>
                              </div>
                            </div>
                            <div class="preview-item-content d-sm-flex flex-grow">
                              <div class="flex-grow">
                                <h6 class="preview-subject">Project meeting</h6>
                                <p class="text-muted mb-0">New project discussion</p>
                              </div>
                              <div class="me-auto text-sm-right pt-2 pt-sm-0">
                                <p class="text-muted">35 minutes ago</p>
                                <p class="text-muted mb-0">15 tasks, 2 issues</p>
                              </div>
                            </div>
                          </div>
                          <div class="preview-item border-bottom">
                            <div class="preview-thumbnail">
                              <div class="preview-icon bg-danger">
                                <i class="mdi mdi-email-open"></i>
                              </div>
                            </div>
                            <div class="preview-item-content d-sm-flex flex-grow">
                              <div class="flex-grow">
                                <h6 class="preview-subject">Broadcast Mail</h6>
                                <p class="text-muted mb-0">Sent release details to team</p>
                              </div>
                              <div class="me-auto text-sm-right pt-2 pt-sm-0">
                                <p class="text-muted">55 minutes ago</p>
                                <p class="text-muted mb-0">35 tasks, 7 issues </p>
                              </div>
                            </div>
                          </div>
                          <div class="preview-item">
                            <div class="preview-thumbnail">
                              <div class="preview-icon bg-warning">
                                <i class="mdi mdi-chart-pie"></i>
                              </div>
                            </div>
                            <div class="preview-item-content d-sm-flex flex-grow">
                              <div class="flex-grow">
                                <h6 class="preview-subject">UI Design</h6>
                                <p class="text-muted mb-0">New application planning</p>
                              </div>
                              <div class="me-auto text-sm-right pt-2 pt-sm-0">
                                <p class="text-muted">50 minutes ago</p>
                                <p class="text-muted mb-0">27 tasks, 4 issues </p>
                              </div>
                            </div>
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div> -->



          <div class="row ">
            <div class="col-12 grid-margin">
              <div class="card">
                <div class="card-body">
                  <h4 class="card-title" id="simulations">Past Simulations</h4>
                  <div class="table-responsive">
                    <table class="table">
                      <thead>
                        <tr>
                          <th>
                            <div class="form-check form-check-muted m-0">
                              <label class="form-check-label">
                                <input type="checkbox" class="form-check-input">
                              </label>
                            </div>
                          </th>
                          <th> Name </th>
                          <th> Description </th>
                          <th> Dataset </th>
                          <th> Tile Size </th>
                          <th> Hash Instx Count </th>
                          <!-- <th> Start Date </th> -->
                          <th> Status </th>
                        </tr>
                      </thead>
                      <tbody>


                        <?php
                        // $mongo = new \MongoDB\Client('mongodb://localhost:27017');
                        // Get the UI_DB database
                        $ui_db = $mongo->selectDatabase('UI_DB');
                        // Get the UI_COLLECTION collection
                        $ui_collection = $ui_db->selectCollection('UI_COLLECTION');
                        // Iterate over documents of UI_COLLECTION collection
                        // $cursor = $ui_collection->find();
                        $cursor = $ui_collection->find(['display' => 1], ['sort' => ['_id' => -1]]);
                        foreach ($cursor as $document) {
                          echo "<tr>";
                          echo "<td>";
                          echo "<div class='form-check form-check-muted m-0'>";
                          echo "<label class='form-check-label'>";
                          echo "<input type='checkbox' class='form-check-input'>";
                          echo "</label>";
                          echo "</div>";
                          echo "</td>";
                          echo "<td>";
                          echo "<a href='/pages/deploy/results.php?exp_name=" . $document['exp_name'] . "'> ";
                          echo "<img src='assets/images/file-icons/64/005-database.png' alt='image' />";
                          echo "</a>";
                          echo "<span class='ps-2'> <a href='/pages/deploy/progress.php?exp_name=" . $document['exp_name'] . "'> " . $document['exp_name'] . " </a> </span>";
                          echo "</td>";
                          echo "<td> " . $document['description'] . " </td>";
                          // echo "<td> <a href='/pages/deploy/results.php?exp_name=" . $document['exp_name'] . "'> " . $document['description'] . " </a> </td>";
                          echo "<td> " . $document['dataset'] . " </td>";
                          echo "<td> " . $document['tile_size'] . " </td>";
                          echo "<td> " . $document['hash_instx_count'] . " </td>";
                          // echo "<td> " . $document['start_date'] . " </td>";
                          echo "<td>";
                          echo "<a href='/pages/deploy/results.php?exp_name=" . $document['exp_name'] . "'>";
                          if ($document['completed'] == 0)
                            echo "<div class='badge badge-outline-warning'>Pending</div>";
                          else
                            echo "<div class='badge badge-outline-success'>Completed</div>";

                          echo "</a>";
                          echo "</td>";
                          echo "</tr>";

                        }
                        ?>

                      </tbody>
                    </table>
                  </div>
                </div>
              </div>
            </div>
          </div>





          <!-- <div class="row">
              <div class="col-md-6 col-xl-4 grid-margin stretch-card">
                <div class="card">
                  <div class="card-body">
                    <div class="d-flex flex-row justify-content-between">
                      <h4 class="card-title">Messages</h4>
                      <p class="text-muted mb-1 small">View all</p>
                    </div>
                    <div class="preview-list">
                      <div class="preview-item border-bottom">
                        <div class="preview-thumbnail">
                          <img src="assets/images/faces/face6.jpg" alt="image" class="rounded-circle" />
                        </div>
                        <div class="preview-item-content d-flex flex-grow">
                          <div class="flex-grow">
                            <div class="d-flex d-md-block d-xl-flex justify-content-between">
                              <h6 class="preview-subject">Leonard</h6>
                              <p class="text-muted text-small">5 minutes ago</p>
                            </div>
                            <p class="text-muted">Well, it seems to be working now.</p>
                          </div>
                        </div>
                      </div>
                      <div class="preview-item border-bottom">
                        <div class="preview-thumbnail">
                          <img src="assets/images/faces/face8.jpg" alt="image" class="rounded-circle" />
                        </div>
                        <div class="preview-item-content d-flex flex-grow">
                          <div class="flex-grow">
                            <div class="d-flex d-md-block d-xl-flex justify-content-between">
                              <h6 class="preview-subject">Luella Mills</h6>
                              <p class="text-muted text-small">10 Minutes Ago</p>
                            </div>
                            <p class="text-muted">Well, it seems to be working now.</p>
                          </div>
                        </div>
                      </div>
                      <div class="preview-item border-bottom">
                        <div class="preview-thumbnail">
                          <img src="assets/images/faces/face9.jpg" alt="image" class="rounded-circle" />
                        </div>
                        <div class="preview-item-content d-flex flex-grow">
                          <div class="flex-grow">
                            <div class="d-flex d-md-block d-xl-flex justify-content-between">
                              <h6 class="preview-subject">Ethel Kelly</h6>
                              <p class="text-muted text-small">2 Hours Ago</p>
                            </div>
                            <p class="text-muted">Please review the tickets</p>
                          </div>
                        </div>
                      </div>
                      <div class="preview-item border-bottom">
                        <div class="preview-thumbnail">
                          <img src="assets/images/faces/face11.jpg" alt="image" class="rounded-circle" />
                        </div>
                        <div class="preview-item-content d-flex flex-grow">
                          <div class="flex-grow">
                            <div class="d-flex d-md-block d-xl-flex justify-content-between">
                              <h6 class="preview-subject">Herman May</h6>
                              <p class="text-muted text-small">4 Hours Ago</p>
                            </div>
                            <p class="text-muted">Thanks a lot. It was easy to fix it .</p>
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
              <div class="col-md-6 col-xl-4 grid-margin stretch-card">
                <div class="card">
                  <div class="card-body">
                    <h4 class="card-title">Portfolio Slide</h4>
                    <div class="owl-carousel owl-theme full-width owl-carousel-dash portfolio-carousel" id="owl-carousel-basic">
                      <div class="item">
                        <img src="assets/images/dashboard/Rectangle.jpg" alt="">
                      </div>
                      <div class="item">
                        <img src="assets/images/dashboard/Img_5.jpg" alt="">
                      </div>
                      <div class="item">
                        <img src="assets/images/dashboard/img_6.jpg" alt="">
                      </div>
                    </div>
                    <div class="d-flex py-4">
                      <div class="preview-list w-100">
                        <div class="preview-item p-0">
                          <div class="preview-thumbnail">
                            <img src="assets/images/faces/face12.jpg" class="rounded-circle" alt="">
                          </div>
                          <div class="preview-item-content d-flex flex-grow">
                            <div class="flex-grow">
                              <div class="d-flex d-md-block d-xl-flex justify-content-between">
                                <h6 class="preview-subject">CeeCee Bass</h6>
                                <p class="text-muted text-small">4 Hours Ago</p>
                              </div>
                              <p class="text-muted">Well, it seems to be working now.</p>
                            </div>
                          </div>
                        </div>
                      </div>
                    </div>
                    <p class="text-muted">Well, it seems to be working now. </p>
                    <div class="progress progress-md portfolio-progress">
                      <div class="progress-bar bg-success" role="progressbar" style="width: 50%" aria-valuenow="25" aria-valuemin="0" aria-valuemax="100"></div>
                    </div>
                  </div>
                </div>
              </div>
              <div class="col-md-12 col-xl-4 grid-margin stretch-card">
                <div class="card">
                  <div class="card-body">
                    <h4 class="card-title">To do list</h4>
                    <div class="add-items d-flex">
                      <input type="text" class="form-control todo-list-input" placeholder="enter task..">
                      <button class="add btn btn-primary todo-list-add-btn">Add</button>
                    </div>
                    <div class="list-wrapper">
                      <ul class="d-flex flex-column-reverse text-white todo-list todo-list-custom">
                        <li>
                          <div class="form-check form-check-primary">
                            <label class="form-check-label">
                              <input class="checkbox" type="checkbox"> Create invoice </label>
                          </div>
                          <i class="remove mdi mdi-close-box"></i>
                        </li>
                        <li>
                          <div class="form-check form-check-primary">
                            <label class="form-check-label">
                              <input class="checkbox" type="checkbox"> Meeting with Alita </label>
                          </div>
                          <i class="remove mdi mdi-close-box"></i>
                        </li>
                        <li class="completed">
                          <div class="form-check form-check-primary">
                            <label class="form-check-label">
                              <input class="checkbox" type="checkbox" checked> Prepare for presentation </label>
                          </div>
                          <i class="remove mdi mdi-close-box"></i>
                        </li>
                        <li>
                          <div class="form-check form-check-primary">
                            <label class="form-check-label">
                              <input class="checkbox" type="checkbox"> Plan weekend outing </label>
                          </div>
                          <i class="remove mdi mdi-close-box"></i>
                        </li>
                        <li>
                          <div class="form-check form-check-primary">
                            <label class="form-check-label">
                              <input class="checkbox" type="checkbox"> Pick up kids from school </label>
                          </div>
                          <i class="remove mdi mdi-close-box"></i>
                        </li>
                      </ul>
                    </div>
                  </div>
                </div>
              </div>
            </div> -->



          <!-- <div class="row">
              <div class="col-12">
                <div class="card">
                  <div class="card-body">
                    <h4 class="card-title">Visitors by Countries</h4>
                    <div class="row">
                      <div class="col-md-5">
                        <div class="table-responsive">
                          <table class="table">
                            <tbody>
                              <tr>
                                <td>
                                  <i class="flag-icon flag-icon-us"></i>
                                </td>
                                <td>USA</td>
                                <td class="text-right"> 1500 </td>
                                <td class="text-right font-weight-medium"> 56.35% </td>
                              </tr>
                              <tr>
                                <td>
                                  <i class="flag-icon flag-icon-de"></i>
                                </td>
                                <td>Germany</td>
                                <td class="text-right"> 800 </td>
                                <td class="text-right font-weight-medium"> 33.25% </td>
                              </tr>
                              <tr>
                                <td>
                                  <i class="flag-icon flag-icon-au"></i>
                                </td>
                                <td>Australia</td>
                                <td class="text-right"> 760 </td>
                                <td class="text-right font-weight-medium"> 15.45% </td>
                              </tr>
                              <tr>
                                <td>
                                  <i class="flag-icon flag-icon-gb"></i>
                                </td>
                                <td>United Kingdom</td>
                                <td class="text-right"> 450 </td>
                                <td class="text-right font-weight-medium"> 25.00% </td>
                              </tr>
                              <tr>
                                <td>
                                  <i class="flag-icon flag-icon-ro"></i>
                                </td>
                                <td>Romania</td>
                                <td class="text-right"> 620 </td>
                                <td class="text-right font-weight-medium"> 10.25% </td>
                              </tr>
                              <tr>
                                <td>
                                  <i class="flag-icon flag-icon-br"></i>
                                </td>
                                <td>Brasil</td>
                                <td class="text-right"> 230 </td>
                                <td class="text-right font-weight-medium"> 75.00% </td>
                              </tr>
                            </tbody>
                          </table>
                        </div>
                      </div>
                      <div class="col-md-7">
                        <div id="audience-map" class="vector-map"></div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div> -->


        </div>
        <!-- content-wrapper ends -->
        <!-- partial:partials/_footer.html -->
        <!-- <footer class="footer">
            <div class="d-sm-flex justify-content-center justify-content-sm-between">
              <span class="text-muted d-block text-center text-sm-left d-sm-inline-block">Copyright © bootstrapdash.com 2021</span>
              <span class="float-none float-sm-right d-block mt-1 mt-sm-0 text-center"> Free <a href="https://www.bootstrapdash.com/bootstrap-admin-template/" target="_blank">Bootstrap admin template</a> from Bootstrapdash.com</span>
            </div>
          </footer> -->
        <!-- partial -->
        <footer class="footer">
          <div class="d-sm-flex justify-content-center justify-content-sm-between">
            <span class="text-muted d-block text-center text-sm-left d-sm-inline-block">Made with <i
                class="mdi mdi-heart text-danger"></i> by Kaustubh Shivdikar </span>

            <span class="float-none float-sm-right d-block mt-1 mt-sm-0 text-center">
              <i class="mdi mdi-flask-outline text-danger"></i>
              Patent Pending</span>
          </div>
        </footer>
      </div>
      <!-- main-panel ends -->
    </div>
    <!-- page-body-wrapper ends -->
  </div>
  <!-- container-scroller -->
  <!-- plugins:js -->
  <script src="assets/vendors/js/vendor.bundle.base.js"></script>
  <!-- endinject -->
  <!-- Plugin js for this page -->
  <script src="assets/vendors/chart.js/Chart.min.js"></script>
  <script src="assets/vendors/progressbar.js/progressbar.min.js"></script>
  <script src="assets/vendors/jvectormap/jquery-jvectormap.min.js"></script>
  <script src="assets/vendors/jvectormap/jquery-jvectormap-world-mill-en.js"></script>
  <script src="assets/vendors/owl-carousel-2/owl.carousel.min.js"></script>
  <script src="assets/js/jquery.cookie.js" type="text/javascript"></script>
  <!-- End plugin js for this page -->
  <!-- inject:js -->
  <script src="assets/js/off-canvas.js"></script>
  <script src="assets/js/hoverable-collapse.js"></script>
  <script src="assets/js/misc.js"></script>
  <script src="assets/js/settings.js"></script>
  <script src="assets/js/todolist.js"></script>
  <!-- endinject -->
  <!-- Custom js for this page -->
  <script src="assets/js/dashboard.js"></script>
  <!-- End custom js for this page -->
</body>

</html>