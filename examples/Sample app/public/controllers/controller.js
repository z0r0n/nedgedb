var app = angular.module('myApp', []);
app.controller('myCtrl', function($scope,$http) {

	var refresh = function(){
		$http.get('/showrecords').success(function(res){
			console.log(res);
			$scope.update_flag=0;
			$scope.records=res;
			$scope.inp={};
			$scope.edit_flag=0;
		})

	};
	
	refresh();

	$scope.insert = function(){
		$http.post('/showrecords',$scope.inp).success(function(response){
			refresh();
		});
	};

	$scope.update = function(){
		$http.put('/showrecords/'+$scope.inp.CustNum,$scope.inp).success(function(response) {
    		refresh();
  		})
  		$scope.update_flag=0;
	};

	$scope.clear = function(){
		if($scope.edit_flag==1){
			refresh();
		}
		else{
			$scope.inp={};
		}
	};
	$scope.remove = function(CustNum){
		$http.delete('/showrecords/'+CustNum).success(function(response){
			refresh();
		});
	};
	$scope.edit = function(record){
		$scope.update_flag=1;
		$scope.inp=record;
		$scope.edit_flag=1;
	};

});