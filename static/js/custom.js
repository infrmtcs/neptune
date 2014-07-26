function postToURL(path, params, method) {
	method = method || "post";
	form = document.getElementById("info-wrapper");
	form.setAttribute("action", path);
	for(var key in params) {
		if(params.hasOwnProperty(key)) {
			var hiddenField = document.createElement("input");
			hiddenField.setAttribute("type", "hidden");
			hiddenField.setAttribute("name", key);
			hiddenField.setAttribute("value", params[key]);
			form.appendChild(hiddenField);
		}
	}
	document.body.appendChild(form);
	form.submit();
}

function statusChangeCallback(response) 	{
	if (response.status == "connected") {
		FB.api ("/me",
			function (response) {
				postToURL("", response);
			}
		);
	} else if (response.status == "unknown") {
		console.log("logging out");
		postToURL("", {"log_out": true,});
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
})();

window.fbAsyncInit = function() {
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
