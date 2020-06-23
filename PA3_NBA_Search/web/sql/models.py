from django.db import models

# Create your models here.


class Article(models.Model):
    NID = models.IntegerField()
    title = models.TextField()
    text = models.TextField()
    html = models.TextField()
    src = models.TextField(default='')
    date = models.TextField(default='')


class IndexInfo(models.Model):
    key = models.TextField()
    value = models.TextField()


class TeamInfo(models.Model):
    NID = models.IntegerField()
    name = models.TextField()
    short_name = models.TextField()
    info = models.TextField()
    intro = models.TextField()
    players = models.TextField()  # 现役球员号码列表
    photo = models.TextField()
    heat = models.IntegerField(default=0)


class PlayerInfo(models.Model):
    NID = models.IntegerField()
    name = models.TextField()
    number = models.TextField()
    t_num = models.IntegerField()
    photo = models.TextField()  # 球员照片
