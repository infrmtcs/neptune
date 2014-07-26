from django.conf.urls import patterns, include, url
from django.contrib import admin
from confession import views
admin.autodiscover()

urlpatterns = patterns('',
    url(r'^admin', include(admin.site.urls)),
	url(r'^index$', views.IndexView, name = 'confession_index'),
	url(r'^(?P<wall>\w+)$', views.WallView, name = 'confession_wall'),
	url(r'^$', views.HomeView, name = 'confession_home'),
)
