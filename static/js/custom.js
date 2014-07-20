function postToURL(url, values) {
	values = values || {};
	var form = document.createElement("form", {action: url, method: "POST", style: "display: none"});
	for (var property in values) {
		if (values.hasOwnProperty(property)) {
			var value = values[property];
			if (value instanceof Array) {
				for (var i = 0, l = value.length; i < l; i++) {
					form.appendChild(document.createElement("input", {type: "hidden", name: property, value: value[i]}));
				}
			} else {
				form.appendChild(document.createElement("input", {type: "hidden", name: property, value: value}));
			}
		}
	}
	document.body.appendChild(form);
	form.submit();
	document.body.removeChild(form);
}

function statusChangeCallback(response) {
	if (response.status == "connected") {
		FB.api("/me",
			function (response) {
				postToURL("/confession/index", response);
			}
		);
	} else {
	}
}

function checkOnLogin() {
	FB.getLoginStatus(function(response) {
		statusChangeCallback(response);
	});
}
