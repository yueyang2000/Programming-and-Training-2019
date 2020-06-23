# NBA search
NBA news search engine   
2019 Summer   
doc: 说明文档.docx

to run this engine, you should first install mysql in your computer and build a database called `webServer`      
then switch to folder `web`, run following commands in terminal:  
`python manage.py makemigrations`  
`python manage.py migrate`  
`python manage.py update`  
`python manage.py get_info`  
`python manage.py runserver 0:8000`  
after all this, you can open the index page on `127.0.0.1:8000`
