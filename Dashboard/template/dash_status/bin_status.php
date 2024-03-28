<?php
$bin_ok = shell_exec("../check_bin.sh");


if ($bin_ok == 1) {
	echo json_encode(['status_text' => "Compile OK", 'status_icon' => '<i class="icon-lg mdi mdi-code-tags text-warning ms-auto"></i>', 'shell_result' => $bin_ok]);
} else {
	echo json_encode(['status_text' => "Error", 'status_icon' => '<i class="icon-lg mdi mdi-close text-danger ms-auto"></i>', 'shell_result' => $bin_ok]);
}
?>