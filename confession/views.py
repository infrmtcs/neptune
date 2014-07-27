import datetime
from django.core.urlresolvers import reverse
from django.core.context_processors import csrf
from django.template import RequestContext
from django.db import connection
from django.forms import forms
from django.http import HttpResponseRedirect
from django.shortcuts import render, render_to_response, redirect
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

def checkNewPost(request, wall_owner = None):
	cursor = connection.cursor()
	sql = 'INSERT INTO confession_post \
	(displayed_sender, displayed_sender_link, author, receiver, content, postedtime, deadline, visible) \
	VALUES(%s, %s, %s, %s, %s, %s, %s, %s)'
	if 'fb_id' in request.session:
		author = request.session['fb_id']
	else:
		author = '-1'
	receiver = wall_owner
	content = request.POST['new_content']
	postedtime = datetime.datetime.now()
	deadline = datetime.datetime.now()
	visible = False
	if ('fb_id' in request.session) and ('new_anonymous' not in request.POST):
		displayed_sender = request.session['fullname']
		displayed_sender_link = request.session['link']
	else:
		displayed_sender = 'Anonymous'
		displayed_sender_link = '/'
	cursor.execute(sql, [displayed_sender, displayed_sender_link, author, receiver, content, postedtime, deadline, visible])

def logForHTML(request):
	data_dict = {}
	if 'fb_id' in request.session:
		data_dict['logged_user'] = request.session['fb_id']
		data_dict['logged_user_name'] = request.session['fullname']
		data_dict['logged_user_link'] = request.session['link']
		data_dict['logged'] = True
	else:
		data_dict['logged_user_name'] = 'Anonymous'
		data_dict['logged_user_link'] = '/'
		data_dict['logged'] = False
	return data_dict

def HomeView(request):
	checkUser(request)
	data_dict = logForHTML(request)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	return render_to_response("base.html", data_dict, context_instance) #change to home.html later

def IndexView(request):
	checkUser(request)
	data_dict = logForHTML(request)
	if 'fb_id' not in request.session:
		return redirect('/', data_dict)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	sql = 'SELECT * FROM confession_post WHERE NOT visible AND receiver = \'' + str(request.session['fb_id']) + '\';'
	data_dict['posts_list'] = Post.objects.raw(sql)
	data_dict['wall_name'] = request.session['fullname']
	return render_to_response("index.html", data_dict, context_instance)

def WallView(request, wall = None):
	checkUser(request)
	data_dict = logForHTML(request)
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
			just_post = False
			if 'new_content' in request.POST:
				if request.POST['new_content'] != "":
					just_post = True
					checkNewPost(request, wall_owner)
			sql = 'SELECT * FROM confession_post WHERE visible AND receiver = ' + wall_owner
			posts_list = Post.objects.raw(sql)
			data_dict['posts_list'] = posts_list
			data_dict['just_post'] = just_post
			data_dict['wall_name'] = wall_name
			return render_to_response("wall.html", data_dict, context_instance)
		else:
			return redirect('/') #No such user

def SentView(request):
	checkUser(request)
	data_dict = logForHTML(request)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	return render_to_response("sent.html", data_dict, context_instance)
