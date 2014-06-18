from django.db import models

class User():
    fb_id = models.CharField(max_length = 200)
    
    def __str__(self):
        return self.fb_id
		