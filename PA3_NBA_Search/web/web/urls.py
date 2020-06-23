"""web URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/2.2/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path
from . import views
urlpatterns = [
    path('test', views.test, name='test'),
    path('admin/', admin.site.urls),
    path('', views.index, name='index'),
    path('rank', views.rank, name='rank'),
    path('s', views.search, name='search'),
    path('get_post', views.get_post, name='get_post'),
    path('search_word', views.search_word, name='search_word'),
    path('get_post_brief', views.get_post_brief, name='get_post_brief'),
    path('href_info', views.href_info, name='href_info'),
    path('get_team', views.get_team, name='get_team'),
    path('search_team', views.search_team, name='search_team'),
    path('get_heat', views.get_heat, name='get_heat')
]
