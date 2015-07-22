{
	"targets": [{
		"target_name": "addon",
		"sources": [ "src/nodbc.cpp" ],
		"conditions": [
			['OS == "linux"', {
				'libraries' : [
					'-lodbc'
				]
			}],
			['OS=="win"', {
				'libraries' : [ 
					'-lodbccp32.lib'
				]
			}]
		]
		
	}]
}
