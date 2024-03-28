<!-- <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
<script> -->


<div class="row">
    <div class="col-xl-3 col-sm-6 grid-margin stretch-card">
        <div class="card">
            <div class="card-body">
                <div class="row">
                    <div class="col-9">
                        <div class="d-flex align-items-center align-self-start">

                            <script>
                                $(document).ready(function () {
                                    setInterval(function () {
                                        $.ajax({
                                            url: 'https://neurachip.us/dash_status/cpu_status.php',
                                            type: 'GET',
                                            success: function (data) {
                                                var parsed_data = JSON.parse(data);
                                                $('#ktb_free_memory').html(parsed_data.free_memory);
                                                $('#ktb_cpu_usage').html(parsed_data.cpu_usage);
                                                $('#ktb_disk_usage').html(parsed_data.disk_usage);

                                            },
                                            error: function (error) {
                                                console.log('Error:', error);
                                            }
                                        });
                                    }, 2000); // Every 5 seconds
                                });                          
                            </script>
                            <h3 class="mb-0">
                                <div id="ktb_free_memory">
                                    Checking
                                </div>
                            </h3>

                            <!-- <h3 class="mb-0">Mem Usage Value</h3> -->
                            <p class="text-success ms-2 mb-0 font-weight-medium">GB</p>
                        </div>
                    </div>
                    <div class="col-3">
                        <div class="icon icon-box-success ">
                            <span class="mdi mdi-arrow-top-right icon-item"></span>
                        </div>
                    </div>
                </div>
                <h6 class="text-muted font-weight-normal">Free Memory</h6>
            </div>
        </div>
    </div>
    <div class="col-xl-3 col-sm-6 grid-margin stretch-card">
        <div class="card">
            <div class="card-body">
                <div class="row">
                    <div class="col-9">
                        <div class="d-flex align-items-center align-self-start">
                            <h3 class="mb-0">
                                <div id="ktb_cpu_usage">
                                    Checking
                                </div>
                            </h3>

                            <!-- <h3 class="mb-0">$17.34</h3> -->
                            <p class="text-success ms-2 mb-0 font-weight-medium">%</p>
                        </div>
                    </div>
                    <div class="col-3">
                        <div class="icon icon-box-success">
                            <span class="mdi mdi-arrow-top-right icon-item"></span>
                        </div>
                    </div>
                </div>
                <h6 class="text-muted font-weight-normal">CPU Usage</h6>
            </div>
        </div>
    </div>
    <div class="col-xl-3 col-sm-6 grid-margin stretch-card">
        <div class="card">
            <div class="card-body">
                <div class="row">
                    <div class="col-9">
                        <div class="d-flex align-items-center align-self-start">

                            <h3 class="mb-0">
                                <div id="ktb_disk_usage">
                                    Checking
                                </div>
                            </h3>
                            <!-- <h3 class="mb-0">$12.34</h3> -->
                            <p class="text-danger ms-2 mb-0 font-weight-medium">GB</p>
                        </div>
                    </div>
                    <div class="col-3">
                        <div class="icon icon-box-danger">
                            <span class="mdi mdi-arrow-bottom-left icon-item"></span>
                        </div>
                    </div>
                </div>
                <h6 class="text-muted font-weight-normal">Disk Available</h6>
            </div>
        </div>
    </div>
    <div class="col-xl-3 col-sm-6 grid-margin stretch-card">
        <div class="card">
            <div class="card-body">
                <div class="row">
                    <div class="col-9">
                        <div class="d-flex align-items-center align-self-start">
                            <?php
                            $uptime_1 = shell_exec('uptime -p | cut -d"," -f1');
                            $uptime_2 = shell_exec('uptime -p | cut -d"," -f2');
                            echo '<h3 class="mb-0">' . $uptime_1 . '</h3>';
                            ?>
                            <!-- <h3 class="mb-0">$31.53</h3> -->
                            <p class="text-success ms-2 mb-0 font-weight-medium">
                                <?php echo $uptime_2; ?>
                            </p>
                        </div>
                    </div>
                    <div class="col-3">
                        <div class="icon icon-box-success ">
                            <span class="mdi mdi-arrow-top-right icon-item"></span>
                        </div>
                    </div>
                </div>
                <h6 class="text-muted font-weight-normal">Uptime</h6>
            </div>
        </div>
    </div>
</div>