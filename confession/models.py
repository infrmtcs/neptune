#-*- encoding=UTF-8 -*-
from django.db import models
from django import forms
import datetime

class User(models.Model):
	fb_id = models.CharField(max_length = 100)
	fullname = models.CharField(max_length = 100)
	postcount = models.IntegerField(default = 0)
	link = models.CharField(max_length = 100)
	
	def __str__(self):
		return self.fb_id

class Post(models.Model):
	displayed_sender = models.CharField(max_length = 100)
	author = models.CharField(max_length = 100)
	receiver = models.CharField(max_length = 100)
	content = models.CharField(max_length = 1000)
	postedtime = models.DateTimeField()
	deadline = models.DateTimeField()
	visible = models.BooleanField()
	def __str__(self):
		return self.content
