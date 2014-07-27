#-*- encoding=UTF-8 -*-
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
					cursor = connection.cursor()
					sql = 'UPDATE confession_user SET fullname = %s WHERE fb_id = %s';
					cursor.execute(sql, [request.POST['name'], request.POST['id']]);
					cursor = connection.cursor()
					sql = 'UPDATE confession_post SET displayed_sender = %s WHERE author = %s';
					cursor.execute(sql, [request.POST['name'], request.POST['id']]);
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
	(displayed_sender, displayed_sender_link, author, receiver, content, comment, postedtime, visible) \
	VALUES(%s, %s, %s, %s, %s, %s, %s, %s)'
	if 'fb_id' in request.session:
		author = request.session['fb_id']
	else:
		author = '-1'
	receiver = wall_owner
	content = request.POST['new_content']
	comment = u"×"
	postedtime = datetime.datetime.now()
	visible = False
	if ('fb_id' in request.session) and ('new_anonymous' not in request.POST):
		displayed_sender = request.session['fullname']
		displayed_sender_link = request.session['link']
	else:
		displayed_sender = 'Anonymous'
		displayed_sender_link = '/'
	cursor.execute(sql, [displayed_sender, displayed_sender_link, author, receiver, content, comment, postedtime, visible])

def checkNewComment(request):
	visible = True
	comment = request.POST['new_comment']
	new_id = request.POST['new_id']
	sql = 'UPDATE confession_post SET visible = %s, comment = %s WHERE id = %s';
	cursor = connection.cursor()
	cursor.execute(sql, [visible, comment, new_id])

def checkNewDelete(request):
	new_id = request.POST['new_id']
	sql = 'DELETE FROM confession_post WHERE id = %s';
	cursor = connection.cursor()
	cursor.execute(sql, [new_id])

def checkNewReveal(request):
	new_id = request.POST['new_id']
	sql = 'SELECT * FROM confession_post WHERE id = %s'
	posts_list = Post.objects.raw(sql, [new_id])
	for post in posts_list:
		author = post.author
		break
	sql = 'SELECT * FROM confession_user WHERE fb_id = %s'
	users_list = User.objects.raw(sql, [author])
	for user in users_list:
		sql = 'UPDATE confession_post SET displayed_sender = %s, displayed_sender_link = %s WHERE id = %s';
		cursor = connection.cursor()
		cursor.execute(sql, [user.fullname, user.link, new_id])
		break

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
	return render_to_response("home.html", data_dict, context_instance) #change to home.html later

def IndexView(request):
	checkUser(request)
	data_dict = logForHTML(request)
	if 'fb_id' not in request.session:
		return redirect('/', data_dict)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	if 'new_comment' in request.POST:
		checkNewComment(request)
	if 'new_delete' in request.POST:
		checkNewDelete(request)
	if 'new_reveal' in request.POST:
		checkNewReveal(request)
	sql = 'SELECT * FROM confession_post WHERE NOT visible AND receiver = %s ORDER BY postedtime DESC'
	posts_list = Post.objects.raw(sql, str(request.session['fb_id']))
	exist_post = False
	for postss in posts_list:
		exist_post = True
		break
	if exist_post:
		data_dict['posts_list'] = posts_list
	data_dict['wall_name'] = request.session['fullname']
	data_dict['wall_link'] = request.session['link']
	return render_to_response("index.html", data_dict, context_instance)

def WallView(request, wall):
	checkUser(request)
	data_dict = logForHTML(request)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	if 'new_delete' in request.POST:
		checkNewDelete(request)
		data_dict['just_delete'] = True
	if 'new_reveal' in request.POST:
		checkNewReveal(request)
	sql = 'SELECT * FROM confession_user WHERE link = %s';
	users = User.objects.raw(sql, str(wall))
	if len(list(users)):
		for user in users:
			wall_owner = user.fb_id
			wall_name = user.fullname
			wall_link = user.link
			break
		if 'new_content' in request.POST:
			if request.POST['new_content'] != "":
				data_dict['just_post'] = True
				checkNewPost(request, wall_owner)
		sql = 'SELECT * FROM confession_post WHERE visible AND receiver = %s ORDER BY postedtime DESC'
		posts_list = Post.objects.raw(sql, wall_owner)
		if posts_list:
			data_dict['posts_list'] = posts_list			
		data_dict['wall_name'] = wall_name + '\'s wall'
		data_dict['wall_link'] = wall_link
		return render_to_response("wall.html", data_dict, context_instance)
	else:
		return redirect('/') #No such user

def PostView(request, post_id):
	checkUser(request)
	data_dict = logForHTML(request)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	if 'new_comment' in request.POST:
		checkNewComment(request)
	if 'new_delete' in request.POST:
		checkNewDelete(request)
	if 'new_reveal' in request.POST:
		checkNewReveal(request)
	sql = 'SELECT * FROM confession_post WHERE visible AND id = %s'
	posts_list = Post.objects.raw(sql, post_id)
	exist_post = False
	for postss in posts_list:
		print postss.receiver
		sql = 'SELECT * FROM confession_user WHERE fb_id = %s'
		users = User.objects.raw(sql, postss.receiver)
		exist_post = True
		break
	if exist_post:
		for userss in users:
			data_dict['posts_list'] = posts_list
			data_dict['wall_name'] = 'A post on ' + userss.fullname + '\'s wall'
			data_dict['wall_link'] = userss.link
			break
	return render_to_response("index.html", data_dict, context_instance)

def SentView(request):
	checkUser(request)
	data_dict = logForHTML(request)
	if 'fb_id' not in request.session:
		return redirect('/', data_dict)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	if 'new_comment' in request.POST:
		checkNewComment(request)
	if 'new_delete' in request.POST:
		checkNewDelete(request)
	if 'new_reveal' in request.POST:
		checkNewReveal(request)
	sql = 'SELECT * FROM confession_post WHERE author = %s ORDER BY postedtime DESC'
	posts_list = Post.objects.raw(sql, str(request.session['fb_id']))
	exist_post = False
	for postss in posts_list:
		exist_post = True
		break
	if exist_post:
		data_dict['posts_list'] = posts_list
	data_dict['wall_name'] = 'Sent by ' + request.session['fullname']
	data_dict['wall_link'] = request.session['link']
	return render_to_response("index.html", data_dict, context_instance)

def SettingView(request):
	checkUser(request)
	data_dict = logForHTML(request)
	data_dict['wall_name'] = 'Setting - ' + request.session['fullname']
	if 'fb_id' not in request.session:
		return redirect('/', data_dict)
	context_instance = RequestContext(request)
	context_instance.update(csrf(request))
	if 'new_link' in request.POST:
		can_change = True
		if request.POST['new_link'] not in ['admin', 'index', 'post', 'sent', 'setting']:
			sql = 'SELECT * FROM confession_user WHERE link = %s'
			users = User.objects.raw(sql, request.POST['new_link'])
			for user in users:
				can_change = False
		else:
			can_change = False
		if can_change:
			cursor = connection.cursor()
			sql = 'UPDATE confession_post SET displayed_sender_link = %s WHERE author = %s';
			cursor.execute(sql, [request.POST['new_link'], request.session['fb_id']]);
			cursor = connection.cursor()
			sql = 'UPDATE confession_user SET link = %s WHERE fb_id = %s';
			cursor.execute(sql, [request.POST['new_link'], request.session['fb_id']]);
			data_dict['logged_user_link'] = request.POST['new_link']
			request.session['link'] = request.POST['new_link']
			data_dict['success_change'] = True
		else:
			data_dict['fail_change'] = True
	return render_to_response("setting.html", data_dict, context_instance)
