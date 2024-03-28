<?php
$free_memory_gb = shell_exec('free -g | grep Mem | awk \'{print $4}\'');

// $free_memory_gb = shell_exec('pwd');

$cpu_usage = shell_exec('top -bn1 | grep load | awk \'{printf "%.2f%", $(NF-2)}\'');
// $cpu_usage = shell_exec('mpstat -P ALL 1 1 | awk \'BEGIN{sum=0} $3 ~ /[0-9.]+/ {sum+=$4+$6} END {printf "%.2f%%", sum}\'');
$disk_usage = shell_exec('df -h | grep "/dev/nvme2n1p2" | awk \'{print $4}\' | cut -d"G" -f1');
// echo '<h3 class="mb-0">' . $free_memory_gb . '</h3>';

echo json_encode(	['free_memory' => $free_memory_gb,
					'cpu_usage' => $cpu_usage,
					'disk_usage' => $disk_usage]);
?>