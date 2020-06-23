from requests_html import HTMLSession
import json


def get_news_page(page):
    session = HTMLSession()
    r = session.get('https://voice.hupu.com/nba/{}'.format(page))
    news = r.html.find('div.news-list', first=True)
    lists = news.find('li')
    news_urls = []
    for li in lists:
        url = li.find('a', first=True).attrs['href']
        # print(url)
        news_urls.append(url)
    return news_urls


def read_news(url):
    session = HTMLSession()
    r = session.get(url)
    title = r.html.find('div.artical-title',
                        first=True).find('h1', first=True).text
    # content store in html format
    content = r.html.find('div.artical-content', first=True)
    date = r.html.find('#pubtime_baidu', first=True).text
    print(title, date)
    return title, content.html, content.text, date


def read_json():
    f = open("test.json", "r", encoding="utf-8")
    s = f.read()
    data = json.loads(s)
    # print(data)


def main():
    '''
    for url in get_team_urls():
        get_team_details(url)
    '''
    news_urls = []
    for i in range(1, 101):
        print('page' + str(i))
        news_urls += get_news_page(i)

    data = {}
    cnt = 0
    for url in news_urls:
        title, html, text, date = read_news(url)
        data[cnt] = dict()
        data[cnt]['title'] = title
        data[cnt]['text'] = text
        data[cnt]['html'] = html
        data[cnt]['src'] = url
        data[cnt]['date'] = date
        cnt += 1
        print("news No." + str(cnt))

    s = json.dumps(data, ensure_ascii=False)
    f = open("test.json", "w", encoding="utf-8")
    f.write(s)
    # read_json()


main()
