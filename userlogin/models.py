from django.db import models

class User(models.Model):
    fb_id = models.CharField(max_length = 200)
    postcount = models.IntegerField(default = 0)
    
    def __str__(self):
        return self.fb_id
		