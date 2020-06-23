from django.shortcuts import render
from django.http import JsonResponse
from django.http import HttpResponse
from django.http import Http404
from sql.models import *
import json
import jieba
import jieba.analyse
import time


def test(request):
    return render(request, 'test.html')


def index(request):
    return render(request, 'index.html')


def rank(request):
    return render(request, 'rank.html')


def get_post(request):
    tID = request.GET.get('id', '')
    if not tID:
        return Http404('nothing to post...')
    data = Article.objects.get(NID=int(tID))
    if not data:
        return Http404('post not found!')
    return render(request, 'post.html', {
        'NID': data.NID,
        'title': data.title,
        'content': data.html,
        'src': data.src,
        'date': data.date
    })


def get_team(request):
    tID = request.GET.get('id', '')
    if not tID:
        return Http404('team not found!')
    data = TeamInfo.objects.get(NID=int(tID))
    if not data:
        return Http404('team not found!')
    result = dict()
    result['NID'] = str(data.NID)
    result['name'] = data.name
    result['short_name'] = data.short_name
    infos = data.info.split('\n')
    for i, info in enumerate(infos):
        txt = 'info' + str(i + 1)
        print(info)
        result[txt] = info
    result['intro'] = data.intro
    result['photo'] = data.photo
    li = list(json.loads(data.players))
    for i, p in enumerate(li):
        player = PlayerInfo.objects.get(NID=p)
        txt = 'player' + str(i + 1)
        if player.number == "":
            result[txt] = player.name
        else:
            result[txt] = player.number + "号 " + player.name
        result[txt + '_photo'] = player.photo
    return render(request, 'team.html', result)


def get_post_brief(request):
    l = len(request.GET)
    if not l:
        return JsonResponse({})
    data = dict()
    for i in range(l):
        x = request.GET.get(str(i))
        tmp = Article.objects.get(NID=x)
        data[i] = {
            'NID': tmp.NID,
            'title': tmp.title,
            'brief': str(tmp.text)[:100] + "..."
        }
    # print(data)
    return JsonResponse(data)


def search(request):
    s = request.GET.get('wd', '')
    if not s:
        return render(request, "index.html")
    return render(request, "search.html", {'wd': s})


def total(request):
    return JsonResponse({'total': PostInfo.objects.count()})


def search_word(request):
    short_names = ['老鹰', '凯尔特人', '公牛', '独行侠', '掘金', '勇士', '黄蜂', '篮网', '骑士', '火箭',
                   '森林狼', '快船', '热火', '尼克斯', '活塞', '灰熊', '雷霆', '湖人',
                   '魔术', '76人', '步行者', '鹈鹕', '开拓者', '太阳', '奇才', '猛龙', '雄鹿', '马刺', '爵士', '国王']
    for name in short_names:
        jieba.add_word(name)
    s = request.GET.get('wd', '')
    if not s:
        return JsonResponse({})

    ans = []
    startTime = time.time()
    # ss = jieba.cut(''.join(s.split()))
    rwords = []
    words = jieba.cut(s)
    for word in words:
        rwords.append(word)
    # 返回盲切标红

    keywords = jieba.analyse.extract_tags(s, topK=5, withWeight=True)
    result = dict()
    for item in keywords:
        word = item[0]
        w = item[1]
        # print(word, w)
        try:
            obj = IndexInfo.objects.get(key=word)
        except Exception as e:
            continue
        arr = list(json.loads(obj.value))
        for x in arr:
            if result.get(x[1]):
                result[x[1]] += x[0] * w
            else:
                result[x[1]] = x[0] * w
    result = list(result.items())
    result.sort(key=lambda x: x[1], reverse=True)  # 按权重倒序排
    ans = [result[i][0] for i in range(len(result))]

    # 如果没有结果，再匹配每个词
    if len(ans) == 0:
        result = dict()
        for word in rwords:
            print(word)
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
        result.sort(key=lambda x: x[1], reverse=True)  # 按权重倒序排
        ans = [result[i][0] for i in range(len(result))]
    for word in rwords:
        if word.strip() == '':
            rwords.remove(word)
        # print("search result:", ans)
    return JsonResponse({
        'len': len(ans),
        'cost': '{:.5f}'.format(time.time() - startTime),
        'word': rwords, 'data': ans})


def search_team(request):
    short_names = ['老鹰', '凯尔特人', '公牛', '独行侠', '掘金', '勇士', '黄蜂', '篮网', '骑士', '火箭',
                   '森林狼', '快船', '热火', '尼克斯', '活塞', '灰熊', '雷霆', '湖人',
                   '魔术', '76人', '步行者', '鹈鹕', '开拓者', '太阳', '奇才', '猛龙', '雄鹿', '马刺', '爵士', '国王']

    for name in short_names:
        jieba.add_word(name)
    s = request.GET.get('wd', '')
    if not s:
        JsonResponse({})

    team = TeamInfo.objects.get(short_name=s)
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
        w = int()
        if word == team.short_name:
            w = 3
        else:
            w = 1
        try:
            obj = IndexInfo.objects.get(key=word)
        except Exception as e:
            continue
        arr = list(json.loads(obj.value))
        for x in arr:
            if result.get(x[1]):
                result[x[1]] += x[0] * w
            else:
                result[x[1]] = x[0] * w
    result = list(result.items())
    result.sort(key=lambda x: x[1], reverse=True)  # 按权重倒序排
    ans = [result[i][0] for i in range(len(result))]
    # print(ans)
    return JsonResponse({
        'len': len(ans),
        'word': rwords,
        'data': ans
    })
# 获得字串-球队映射


def href_info(request):
    data = dict()
    players = PlayerInfo.objects.all()
    for player in players:
        data[player.name] = player.t_num
    teams = TeamInfo.objects.all()
    for team in teams:
        data[team.short_name] = team.NID
    # print(data)
    return JsonResponse(data)


def get_heat(request):
    ans = []
    teams = TeamInfo.objects.all()
    for team in teams:
        t = dict()
        t['NID'] = team.NID
        t['name'] = team.name
        t['heat'] = team.heat
        t['photo'] = team.photo
        t['info'] = team.intro[:50] + '...'
        ans.append(t)
    ans.sort(key=lambda x: x['heat'], reverse=True)
    ret = {}
    for i, a in enumerate(ans):
        ret[i] = a
    return JsonResponse({'data': ret})
