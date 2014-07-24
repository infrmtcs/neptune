#-*- encoding=UTF-8 -*-
from django.db import models
from django.forms import DateField
import datetime

class User(models.Model):
	fb_id = models.CharField(max_length = 100)
	fullname = models.CharField(max_length = 100)
	postcount = models.IntegerField(default = 0)
	link = models.CharField(max_length = 100)
	
	def __str__(self):
		return self.fb_id
		
class Post(models.Model):
	author = models.ForeignKey(User, related_name = 'author')
	receiver = models.ForeignKey(User, related_name = 'receiver')
	content = models.TextField()
	postedtime = models.DateTimeField()
	deadline = models.DateTimeField()
	visible = models.BooleanField()
	def __str__(self):
		return self.content

