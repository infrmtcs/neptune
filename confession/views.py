import datetime
from django.core.urlresolvers import reverse
from django.db import connection
from django.forms import forms
from django.http import HttpResponseRedirect
from django.shortcuts import render

# Create your views here.
from django.views import generic
from confession.models import Post, User

# class PostView(generic.CreateView):
#     template_name = 'post.html'
#     context_object_name = 'posts_list'
#     model = Post

# class UserForm(forms.Form):
#     model = Post
#     fields = ('content', 'author', 'receiver')

	# def save(self, commit=True):
	#     self.instance.postedtime = datetime.datetime.now();
	#     if self.instance.pk == None:
	#         self.instance.deadline = \
	#             datetime.datetime.now() + datetime.timedelta(hours=3)
	#     return super(UserForm, self).save(commit)

def checkUser(request):
	if 'fb_id' in request.session:
		return
	if 'id' in request.POST and request.POST['id']:
		sql = 'SELECT * FROM userlogin_user WHERE fb_id = \"'+ request.POST['id'] + '\";'
		users = User.objects.raw(sql);
		if len(list(users)):
			pass
		else:
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


class IndexView(generic.ListView):
	template_name = 'index.html'
	context_object_name = 'posts_list'
	model = Post

	def get_queryset(self):
		return Post.objects.values()
