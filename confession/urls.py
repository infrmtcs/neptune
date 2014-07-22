from django.conf.urls import patterns, url
from confession import views

urlpatterns = patterns(
	'',
	url(r'post$', views.PostView.as_view(), name = 'confession_post'),
	url(r'index$', views.IndexView, name = 'confession_index'),
)
