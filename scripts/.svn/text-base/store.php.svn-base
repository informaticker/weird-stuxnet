<?php

############################################
#
# Author: Fabian Schläpfer
# Date:   Jul 7th 2010
#
# This script parses POST data and stores
# the data to file corresponding to
# individuals
#
# FORMAT: RUNID;MODE;[GEN#;[IND#;[LIFECYCLE#;]]][DATA;]


// variable definitions
$folder = "/root/data";

// we need post data
if( $_SERVER['REQUEST_METHOD'] != POST ) {
        die( "Error: Need POST data!" );
}

// check if we have all the data we need
if( !isset($_POST['data']) ) {
        die( "Error: Missing data!\nDump:<pre>" . print_r($_POST,true) . "</pre>\n" );
}

$parts = split( ";", $_POST['data'] );

$ip = $_SERVER['REMOTE_ADDR'];
$runid = $parts[0];
$mode = $parts[1];

$runfolder = $folder . "/" . $ip . "_" . $runid;
		
switch( $mode ) {

	case "run_started": {
		$datetime = $parts[2];
		$runname = $parts[3];
		
		if( is_dir($runfolder) ) {
			die( "Folder '$runfolder' already exists! Something's wrong..." );
		}
		
		$data = $datetime . " " . $runname;
		$file = $runfolder . "/RUNINFO";

		// create folder to contain data for this run
		system( "mkdir -p $runfolder" );
		
		// write run information to file in runfolder
		$f = fopen( $file, "w" );
		if( ! fwrite( $f, $data ) ) {
			die( "Error while writing to file '$file'!" );
		}
		fclose($f);
		
		break;
	}
	
	case "genome": {
		$genid = $parts[2];
		$indid = $parts[3];
		$genome = $parts[4];
		
		// individual's folder
		$indfolder = $runfolder . "/generation" . $genid . "/individuals/individual" . $indid;
		
		// create folder
		system( "mkdir -p " . $indfolder );
	
		$file = $indfolder . "/GENOME";
		$f = fopen( $file, "w" );
		if( ! fwrite( $f, $genome ) ) {
			die( "Error while writing genome to file '$file'!" );
		}
		fclose($f);
	
		break;
	}
	
	case "phenotype": {
		$genid = $parts[2];
		$indid = $parts[3];
		$phenotype = $parts[4];
		
		// individual's folder
		$indfolder = $runfolder . "/generation" . $genid . "/individuals/individual" . $indid;
		
		// create folder
		system( "mkdir -p " . $indfolder );
	
		$file = $indfolder . "/PHENOTYPE";
		$f = fopen( $file, "w" );
		if( ! fwrite( $f, $phenotype ) ) {
			die( "Error while writing phenotype to file '$file'!" );
		}
		fclose($f);
	
		break;
	}


	case "cmd": {
		$ret = $parts[2];
		$genid = $parts[3];
		$lcid = $parts[4];
		$indid = $parts[5];
		$cwd = $parts[6];
		$cmd = $parts[7];
		
		// individual's command collection
		$cmdfolder = $runfolder . "/generation" . $genid . "/individuals/individual" . $indid . "/commands_" . $ret;
		
		// create folder
		system( "mkdir -p " . $cmdfolder );
		
		// CMD
		// write cmd to file
		$file = $cmdfolder . "/lifecycle" . $lcid . "_cmd";
		$data = base64_decode($cmd);
		$f = fopen( $file, "w" );
		if( ! fwrite( $f, $data ) ) {
			die( "Error while writing CMD to file '$file'!" );
		}
		fclose($f);
		
		// CWD
		// write cwd to file
		$file = $cmdfolder . "/lifecycle" . $lcid . "_cwd";
		$data = base64_decode($cwd);
		$f = fopen( $file, "w" );
		if( ! fwrite( $f, $data ) ) {
			die( "Error while writing CWD to file '$file'!" );
		}
		fclose($f);
	
		break;
	}
	
	case "fitness": {
		$genid = $parts[2];
		$indid = $parts[3];
		$fitness = $parts[4];
		
		// individual's folder
		$indfolder = $runfolder . "/generation" . $genid . "/individuals/individual" . $indid;
		
		// create folder
		system( "mkdir -p " . $indfolder );
		
		// write fitness to file
		$file = $indfolder . "/FITNESS";
		$data = $fitness;
		$f = fopen( $file, "w" );
		if( ! fwrite( $f, $data ) ) {
			die( "Error while writing FITNESS to file '$file'!" );
		}
		fclose($f);
	
		break;
	}
	
}

?>


