import datetime
from django.core.urlresolvers import reverse
from django.db import connection
from django.forms import forms
from django.http import HttpResponseRedirect
from django.shortcuts import render, redirect

# Create your views here.
from django.views import generic
from userlogin.models import User

def checkUser(request):
	if 'fb_id' in request.session:
		return
	if 'id' in request.POST and request.POST['id']:
		sql = 'SELECT * FROM userlogin_user WHERE fb_id = \"'+ request.POST['id'] + '\";'
		users = User.objects.raw(sql);
		if len(list(users)):
			return
		cursor = connection.cursor()
		sql = 'INSERT INTO userlogin_user(fb_id, fullname, timezone, postcount) VALUES(\"' \
			  + request.POST['id'] + '\",\"' \
			  + request.POST['name'] + '\", ' \
			  + request.POST['timezone'] + ', 0'
		cursor.execute(sql)
		sql = 'SELECT * FROM userlogin_user WHERE fb_id = \"'+ request.POST['id'] + '\";'
		users = User.objects.raw(sql);
		for i in users:
			request.session['fb_id'] = i.fb_id
			break
	else:
		pass

def register(request):
	if 'username' in request.POST and request.POST['username']:
		if 'password' in request.POST and request.POST['password']:
			if 'password_again' in request.POST and request.POST['password_again']:
				if request.POST['password'] == request.POST['password_again']:
					sql = 'SELECT * FROM userlogin_user WHERE username="'\
						  + request.POST['username'] \
						  + '";'
					users = User.objects.raw(sql)
					if len(list(users)):
						return render(request, 'userlogin/register.html')

					cursor = connection.cursor()
					sql = 'INSERT INTO userlogin_user(username, password) VALUES("' \
						  + request.POST['username'] + '","' \
						  + request.POST['password'] + '");'
					cursor.execute(sql)
					return redirect('/user/login', 'userlogin/index.html')
		return render(request, 'userlogin/register.html')
	else:
		return render(request, 'userlogin/register.html')
