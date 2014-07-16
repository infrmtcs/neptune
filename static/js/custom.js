function statusChangeCallback(response) {
	if (response.status == "connected") {
		console.log(response.authResponse.userID);
	} else {
	}
}

function checkOnLogin() {
	FB.getLoginStatus(function(response) {
		statusChangeCallback(response);
	});
}
