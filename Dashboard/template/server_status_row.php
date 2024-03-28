<!-- 
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
<script> -->

<div class="row">
    <div class="col-sm-4 grid-margin">
        <div class="card">
            <div class="card-body">
                <h5>Server Latency</h5>
                <div class="row">
                    <div class="col-8 col-sm-12 col-xl-8 my-auto">
                        <div class="d-flex d-sm-block d-md-flex align-items-center">
                            <script>
                                $(document).ready(function () {
                                    setInterval(function () {
                                        $.ajax({
                                            url: 'https://neurachip.us/dash_status/latency_update.php',
                                            type: 'GET',
                                            success: function (data) {
                                                var parsed_data = JSON.parse(data);
                                                $('#ktb_latency').html(parsed_data.latency);
                                            },
                                            error: function (error) {
                                                console.log('Error:', error);
                                            }
                                        });
                                    }, 1000); // Every 1 second
                                });                          
                            </script>
                            <h2 class="mb-0">
                                <div id="ktb_latency">
                                    0.0
                                </div>
                            </h2>

                            <!-- <h2 class="mb-0">$32123</h2> -->
                            <p class="text-success ms-2 mb-0 font-weight-medium">ms</p>
                        </div>
                        <h6 class="text-muted font-weight-normal">Server Online</h6>
                    </div>
                    <div class="col-4 col-sm-12 col-xl-4 text-center text-xl-right">
                        <i class="icon-lg mdi mdi-codepen text-primary ms-auto"></i>
                    </div>
                </div>
            </div>
        </div>
    </div>
    <div class="col-sm-4 grid-margin">
        <div class="card">
            <div class="card-body">
                <h5>MongoDB</h5>
                <div class="row">
                    <div class="col-8 col-sm-12 col-xl-8 my-auto">
                        <div class="d-flex d-sm-block d-md-flex align-items-center">

                            <!-- <script>
                            $(document).ready(function() {
                              setInterval(function() {
                                  $.ajax({
                                      url: 'dash_status/mongo_status.php',
                                      type: 'GET',
                                      success: function(server_data) {
                                          var parsed_server_data = JSON.parse(server_data);
                                          $('#ktb_server_status').html(parsed_server_data.status);
                                          $('#ktb_server_status_icon').html(parsed_server_data.server_icon);
                                      },
                                      error: function(error) {
                                          console.log('Error:', error);
                                      }
                                  });
                              }, 2000); // Every 2 seconds
                          });                          
                        </script>  -->

                            <?php
                            try {
                                //                            $mongo = new \MongoDB\Client('mongodb://localhost:27017');
                                $dbs = $mongo->listDatabases();
                                $mongo_live = 1;
                                // echo "KTB - Mongo Status - 1";
                            } catch (Exception $e) {
                                echo $e->getMessage();
                                // echo "KTB - Mongo Status - 0";
                                $mongo_live = 0;
                            }
                            // if ($mongo_live == 1) {
                            //   echo json_encode(['mongo_live' => $mongo_live, 'status' => 'Online', 'server_icon' => '<i class="icon-lg mdi mdi-server text-success ms-auto"></i>']);
                            // } else {
                            //   echo json_encode(['mongo_live' => $mongo_live, 'status' => 'Offline', 'server_icon' => '<i class="icon-lg mdi mdi-server-off text-danger ms-auto"></i>']);
                            // }
                            ?>

                            <h2 class="mb-0">
                                <?php
                                if ($mongo_live == 1) {
                                    echo "Online";
                                } else {
                                    echo "Offline";
                                }
                                // <div id="ktb_server_status">
                                //   Checking
                                // </div>
                                ?>
                            </h2>
                            <p class="text-success ms-2 mb-0 font-weight-medium">Port: Hidden</p>
                        </div>
                        <h6 class="text-muted font-weight-normal">Private Access Only</h6>
                    </div>
                    <div class="col-4 col-sm-12 col-xl-4 text-center text-xl-right">
                        <?php
                        if ($mongo_live == 1) {
                            echo '<i class="icon-lg mdi mdi-server text-success ms-auto"></i>';
                        } else {
                            echo '<i class="icon-lg mdi mdi-server-off text-danger ms-auto"></i>';
                        }
                        ?>
                        <!-- <div id="ktb_server_status_icon">
                          <i class="icon-lg mdi mdi-server-off text-danger ms-auto"></i>
                        </div> -->

                    </div>
                </div>
            </div>
        </div>
    </div>
    <div class="col-sm-4 grid-margin">
        <div class="card">
            <div class="card-body">
                <h5>NeuraSim</h5>
                <div class="row">
                    <div class="col-8 col-sm-12 col-xl-8 my-auto">
                        <div class="d-flex d-sm-block d-md-flex align-items-center">

                            <script>
                                $(document).ready(function () {
                                    setInterval(function () {
                                        $.ajax({
                                            url: '/dash_status/bin_status.php',
                                            type: 'GET',
                                            success: function (bin_data) {
                                                var parsed_bin_data = JSON.parse(bin_data);
                                                $('#ktb_bin_status').html(parsed_bin_data.status_text);
                                                $('#ktb_bin_status_2').html(parsed_bin_data.status_text);
                                                $('#ktb_bin_icon').html(parsed_bin_data.status_icon);


                                            },
                                            error: function (error) {
                                                console.log('Error:', error);
                                            }
                                        });
                                    }, 1000); // Every 2 seconds
                                });                          
                            </script>

                            <h2 class="mb-0">
                                <div id="ktb_bin_status">
                                    Checking
                                </div>
                            </h2>
                            <!-- <h2 class="mb-0">$2039</h2> -->
                            <p class="text-danger ms-2 mb-0 font-weight-medium">chippy</p>
                        </div>
                        <h6 class="text-muted font-weight-normal">
                            <div id="ktb_bin_status_2">
                                Checking
                            </div>
                        </h6>
                        <!-- <p class="text-danger ms-2 mb-0 font-weight-medium">chippy</p> -->

                        <!-- <h6 class="text-muted font-weight-normal">2.27% Since last month</h6> -->
                    </div>
                    <div class="col-4 col-sm-12 col-xl-4 text-center text-xl-right">
                        <div id="ktb_bin_icon">
                            <i class="icon-lg mdi mdi-close text-danger ms-auto"></i>
                        </div>
                        <!-- <i class="icon-lg mdi mdi-monitor text-success ms-auto"></i> -->
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>