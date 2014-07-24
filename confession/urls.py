from django.conf.urls import patterns, url
from confession import views

urlpatterns = patterns(
	'',
	url(r'index$', views.IndexView, name = 'confession_index'),
	url(r'index$', views.WallView, name = 'confession_wall'),
)
