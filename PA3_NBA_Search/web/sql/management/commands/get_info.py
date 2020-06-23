import json
from django.core.management.base import BaseCommand
from django.db import transaction
from sql.models import *
from requests_html import HTMLSession
import json


class Command(BaseCommand):
    short_names = ['老鹰', '凯尔特人', '公牛', '独行侠', '掘金', '勇士', '黄蜂', '篮网', '骑士', '火箭',
                   '森林狼', '快船', '热火', '尼克斯', '活塞', '灰熊', '雷霆', '湖人',
                   '魔术', '76人', '步行者', '鹈鹕', '开拓者', '太阳', '奇才', '猛龙', '雄鹿', '马刺', '爵士', '国王']
    session = HTMLSession()
    team_cnt = 0
    player_cnt = 0
    players = []
    teams = []

    def get_team_urls(self):

        r = self.session.get("https://nba.hupu.com/teams")
        boxs = r.html.find('div.team')
        team_urls = []
        for box in boxs:
            teams = box.find('a')
            for team in teams:
                team_urls.append(team.attrs['href'])
                # print(team.attrs['href'])
        return team_urls

    def get_team_details(self, team_url):
        r = self.session.get(team_url)
        team = dict()
        team['name'] = r.html.find('span.title-text', first=True).text
        for sn in self.short_names:
            if (team['name'].find(sn) != -1):
                if sn == '尼克斯' and team['name'].find('太阳') != -1:
                    team['short_name'] = '太阳'
                    break
                team['short_name'] = sn
                print(sn)
                break
        # 基本信息
        team['info'] = r.html.find('div.clearfix', first=True).text
        # print(info.text)
        # 文字简介
        team['intro'] = r.html.find('div.txt', first=True).text
        # print(intro.text)
        team['photo'] = self.get_photo(team_url)
        team['NID'] = self.team_cnt

        names = []
        numbers = []
        photos = []
        lists = r.html.find('span.c1')
        for li in lists[1:]:
            numbers.append(li.text)
        lists = r.html.find('span.c2')
        for li in lists[1:]:
            names.append(li.text)
            photos.append(self.get_photo(
                li.find('a', first=True).attrs['href']))

        team['players'] = []
        for i, name in enumerate(names):
            player = dict()
            player['name'] = name
            player['number'] = numbers[i]
            player['photo'] = photos[i]
            player['t_num'] = team['NID']
            player['NID'] = self.player_cnt
            team['players'].append(player['NID'])
            self.player_cnt += 1
            self.players.append(player)

        self.team_cnt += 1
        self.teams.append(team)
        # print(team['NID'], team['short_name'], team['players'])
        # print(players)

    def get_photo(self, url):
        r = self.session.get(url)
        imgs = r.html.find('img')
        photo = imgs[1].attrs['src']
        # print(photo)
        return photo

    @transaction.atomic
    def save_data(self):
        for player in self.players:
            p = PlayerInfo()
            p.NID = player['NID']
            p.name = player['name']
            p.number = player['number']
            p.t_num = player['t_num']
            p.photo = player['photo']
            p.save()
            print('save', player['name'])
        for team in self.teams:
            t = TeamInfo()
            t.NID = team['NID']
            t.name = team['name']
            t.short_name = team['short_name']
            t.info = team['info']
            t.intro = team['intro']
            t.players = json.dumps(team['players'])
            t.photo = team['photo']
            t.save()
            print('save', team['name'])

    @transaction.atomic
    def cal_heat(self):
        teams = TeamInfo.objects.all()
        for team in teams:
            players = list(json.loads(team.players))
            # print(players)
            key_players = players[0:5]
            rwords = []  # 返回的高亮名
            rwords.append(team.short_name)
            for p in key_players:
                player = PlayerInfo.objects.get(NID=p)
                names = player.name.split('-')
                lname = names[len(names) - 1]
                rwords.append(lname)
            # 搜索球队名和球员的名
            # print('here')
            result = dict()
            for word in rwords:
                try:
                    obj = IndexInfo.objects.get(key=word)
                except Exception as e:
                    continue
                arr = list(json.loads(obj.value))
                for x in arr:
                    if result.get(x[1]):
                        result[x[1]] += x[0]
                    else:
                        result[x[1]] = x[0]
            result = list(result.items())
            team.heat = len(result)
            team.save()
            print(team.short_name, team.heat)

    def handle(self, *args, **options):
        '''
        for url in self.get_team_urls():
            self.get_team_details(url)
        self.save_data()
        '''
        self.cal_heat()
