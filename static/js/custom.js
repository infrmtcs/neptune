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
				console.log(property + "    " + value);
			}
		}
	}
	// document.body.appendChild(form);
	// form.submit();
	// document.body.removeChild(form);
}

function statusChangeCallback(response) {
	if (response.status == "connected") {
		FB.api("/me",
			function (response) {
				postToURL("/confession/index", response);
			}
		);
	} else {
		console.log("logged out");
	}
}

function checkOnLogin() {
	FB.getLoginStatus(function(response) {
		statusChangeCallback(response);
	});
}


//start
var _gaq = _gaq || [];
_gaq.push(['_setAccount', 'UA-23019901-1']);
_gaq.push(['_setDomainName', "bootswatch.com"]);
_gaq.push(['_setAllowLinker', true]);
_gaq.push(['_trackPageview']);
(function() {
	var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
	ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
	var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
})();window.fbAsyncInit = function() {
	FB.init({
		appId      : '600831223364085',
		cookie     : true,
		xfbml      : true,
		version    : 'v2.0'
	});
};
(function(d, s, id) {
	var js, fjs = d.getElementsByTagName(s)[0];
	if (d.getElementById(id)) return;
	js = d.createElement(s); js.id = id;
	js.src = "http://connect.facebook.net/en_GB/sdk.js#xfbml=1&version=v2.0";
	fjs.parentNode.insertBefore(js, fjs);
}(document, 'script', 'facebook-jssdk'));

!function(d,s,id) {
	var js,fjs=d.getElementsByTagName(s)[0];
	if(!d.getElementById(id)) {
		js=d.createElement(s);js.id=id;
		js.src="https://platform.twitter.com/widgets.js";fjs.parentNode.insertBefore(js,fjs);
	}
}(document,"script","twitter-wjs");