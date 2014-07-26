import datetime
from django.core.urlresolvers import reverse
from django.core.context_processors import csrf
from django.template import RequestContext
from django.db import connection
from django.forms import forms
from django.http import HttpResponseRedirect
from django.shortcuts import render, render_to_response, redirect
from django.views import generic
from confession.models import Post, User


def checkUser(request):
	if 'log_out' in request.POST:
		if 'fb_id' in request.session:
			del request.session['fb_id']
		if 'fullname' in request.session:
			del request.session['fullname']
		if 'link' in request.session:
			del request.session['link']
		# print "logout successfully"
		return
	if 'fb_id' in request.session:
		# print "already logged in"
		return
	if 'id' in request.POST and request.POST['id']:
		sql = 'SELECT * FROM confession_user WHERE fb_id = %s;'
		users = User.objects.raw(sql, [request.POST['id']]);
		if len(list(users)):
			for user in users:
				if user.fullname != request.POST['name']:
					pass
			# print "old user"
		else:
			# print "new user added"
			cursor = connection.cursor()
			sql = 'INSERT INTO confession_user(fb_id, fullname, link, postcount) VALUES(%s, %s, %s, 0)'
			cursor.execute(sql, [request.POST['id'], request.POST['name'], request.POST['id']])
			# print "registered"
		sql = 'SELECT * FROM confession_user WHERE fb_id = %s;'
		users = User.objects.raw(sql, [request.POST['id']]);
		for user in users:
			if user.fb_id == request.POST['id']:
				request.session['link'] = user.link
				request.session['fb_id'] = user.fb_id
				request.session['fullname'] = user.fullname
				request.session['postcount'] = user.postcount
				break
		# print "logged in successfully"
	else:
		# print "no user detected"
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

# def checkNewPost(request):
# 	if 'content' in request.POST:
# 		if request.POST['content'] != "":
# 			cursor = connection.cursor()
# 			sql = 'INSERT INTO confession_post \
# 			(displayed_sender, displayed_sender_link, author, receiver, content, postedtime, deadline, visible) \
# 			VALUES(%s, %s, %s, %s, %s, %s, %s, %s)'
# 			cursor.execute(sql, [])

def HomeView(request):
	checkUser(request)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	return render_to_response("base.html", context_instance) #change to home.html later

def IndexView(request):
	checkUser(request)
	if 'fb_id' not in request.session:
		return HomeView(request)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	sql = 'SELECT * FROM confession_post WHERE NOT visible AND receiver = \'' + str(request.session['fb_id']) + '\';'
	data_dict = {'posts_list': Post.objects.raw(sql), }
	if 'fb_id' in request.session:
		data_dict['logged_user'] = request.session['fb_id']
	return render_to_response("index.html", data_dict, context_instance)

def WallView(request, wall = None):
	checkUser(request)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	if wall is None:
		return redirect('/')
	else:
		sql = 'SELECT * FROM confession_user WHERE link = \'' + str(wall) + '\'';
		users = User.objects.raw(sql)
		if len(list(users)):
			for user in users:
				wall_owner = user.fb_id
				wall_name = user.fullname
				break
			sql = 'SELECT * FROM confession_post WHERE visible AND receiver = ' + wall_owner
			data_dict = {'posts_list': Post.objects.raw(sql), }
			data_dict['wall_name'] = wall_name
			if 'fb_id' in request.session:
				data_dict['logged_user_name'] = request.session['fullname']
				data_dict['logged'] = True
			else:
				data_dict['logged_user_name'] = 'Anonymous'
				data_dict['logged'] = False
			return render_to_response("wall.html", data_dict, context_instance)
		else:
			return redirect('/') #No such user
