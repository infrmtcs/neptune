#-*- encoding=UTF-8 -*-
from django.db import models
from django import forms
import datetime

class User(models.Model):
	fb_id = models.CharField(max_length = 100)
	fullname = models.CharField(max_length = 100)
	postcount = models.IntegerField(default = 0)
	link = models.CharField(max_length = 100)
	
	def __unicode__(self):
		return self.fb_id

class Post(models.Model):
	displayed_sender = models.CharField(max_length = 100)
	displayed_sender_link = models.CharField(max_length = 100)
	author = models.CharField(max_length = 100)
	receiver = models.CharField(max_length = 100)
	content = models.TextField()
	comment = models.TextField()
	postedtime = models.DateTimeField()
	visible = models.BooleanField()
	def __unicode__(self):
		return self.content
