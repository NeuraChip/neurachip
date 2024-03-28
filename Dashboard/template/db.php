<?php
require 'vendor/autoload.php'; // include Composer's autoloader

class DB {
    private static $instance = null;
    private static $client = null;

    private function __construct() {
        self::$client = new MongoDB\Client("mongodb://localhost:27017");
    }

    public static function getInstance() {
        if (!isset(self::$instance)) {
            self::$instance = new DB();
        }
        return self::$client;
    }
}

?>