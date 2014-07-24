import datetime
from django.core.urlresolvers import reverse
from django.core.context_processors import csrf
from django.template import RequestContext
from django.db import connection
from django.forms import forms
from django.http import HttpResponseRedirect
from django.shortcuts import render, render_to_response
from django.views import generic
from confession.models import Post, User


def checkUser(request):
	print "---------------------------------------"
	print request.POST
	print request.session
	print "---------------------------------------"
	if 'log_out' in request.POST:
		if 'fb_id' in request.session:
			del request.session['fb_id']
		if 'fullname' in request.session:
			del request.session['fullname']
		if 'link' in request.session:
			del request.session['link']
		print "logout successfully"
		return
	if 'fb_id' in request.session:
		print "already logged in"
		return
	if 'id' in request.POST and request.POST['id']:
		sql = 'SELECT * FROM confession_user WHERE fb_id = \''+ request.POST['id'] + '\';'
		print sql
		users = User.objects.raw(sql);
		if len(list(users)):
			for user in users:
				if user.fullname != request.POST['name']:
					pass
				request.session['link'] = user.link
				break
			print "old user"
		else:
			print "new user added"
			cursor = connection.cursor()
			sql = 'INSERT INTO confession_user(fb_id, fullname, link, postcount)' \
				+ 'VALUES(\'' + request.POST['id'] + '\', \'' + request.POST['name'] + '\', \'' + request.POST['name'] + '\', 0);'
			print sql
			cursor.execute(sql)
			request.session['link'] = request.POST['id']
			print "registered"
		request.session['fb_id'] = request.POST['id']
		request.session['fullname'] = request.POST['name']
		print "logged in successfully"
	else:
		print "no user detected"
		pass

class PostView(generic.CreateView):
	model = Post
	template_name = 'post.html'
	# form_class = UserForm
	fields = ("content", "author", "receiver")

	def get_success_url(self):
		return reverse('confession_index')

	def form_valid(self, form):
		form.instance.postedtime = datetime.datetime.now()
		form.instance.deadline = datetime.datetime.now()
		return super(PostView, self).form_valid(form)

def IndexView(request, wall=None):
	checkUser(request)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	sql = 'SELECT * FROM confession_post'
	return render_to_response("index.html", {'posts_list': Post.objects.raw(sql),}, context_instance)

def WallView(request, wall=None):
	checkUser(request)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	sql = 'SELECT * FROM confession_post'
	return render_to_response("index.html", {'posts_list': Post.objects.raw(sql),}, context_instance)

# class IndexView(generic.ListView):
# 	template_name = 'index.html'
# 	context_object_name = 'posts_list'
# 	model = Post

# 	def get_queryset(self):
# 		return Post.objects.values()
