from django.db import models
from django.forms import DateField
from userlogin.models import User
import datetime

class Post(models.Model):
    author = models.ForeignKey(User)
    receiver = models.ForeignKey(User)
    content = models.TextField()
    postedtime = models.DateTimeField()
    deadline = models.DateTimeField()
    
    def __str__(self):
        return self.content
