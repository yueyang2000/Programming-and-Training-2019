import json
from django.core.management.base import BaseCommand
from django.db import transaction
from sql.models import *
import jieba


class Command(BaseCommand):
    index = dict()  # 存放索引
    short_names = ['老鹰', '凯尔特人', '公牛', '独行侠', '掘金', '勇士', '黄蜂', '篮网', '骑士', '火箭',
                   '森林狼', '快船', '热火', '尼克斯', '活塞', '灰熊', '雷霆', '湖人',
                   '魔术', '76人', '步行者', '鹈鹕', '开拓者', '太阳', '奇才', '猛龙', '雄鹿', '马刺', '爵士', '国王']

    def cut(self, NID, title, text):
        d = dict()

        def add_val(t, w):
            words = jieba.cut_for_search(t)
            for word in words:
                a = d.get(word)
                if a:
                    d[word] += w
                else:
                    d[word] = w
        add_val(title, 50)
        add_val(text, 1)
        for word, val in d.items():
            a = self.index.get(word)
            t = [val, NID]  # 第一项是词权重
            if a:
                self.index[word].append(t)
            else:
                self.index[word] = [t]

    @transaction.atomic
    def work(self):
        print('----start reading data----')
        f = open('../spider/test.json', 'r')
        all_data = json.loads(f.read())
        cnt = 0
        for NID, data in all_data.items():
            cnt += 1
            print('processing No.{}'.format(NID))
            row = Article()
            row.NID = int(NID)
            row.title = data['title']
            row.text = data['text']
            row.html = data['html']
            row.date = data['date']
            row.src = data['src']
            self.cut(row.NID, row.title, row.text)
            row.save()
        print("total:", cnt)

        print('----building index----')
        for x, y in self.index.items():
            info = IndexInfo()
            info.key = x
            info.value = json.dumps(y)
            info.save()

        print('----index built----')

    def handle(self, *args, **options):
        for name in self.short_names:
            jieba.add_word(name)
        self.work()
