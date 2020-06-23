var post_list = [];
var page_size = 10;
var page_limit = 0;
var page_number = 7;
var page = 0;
var ci = [];
function GoSubmit() {
    console.log('here');
    //$('#search-bg').val($('#bg-time').val());
    //$('#search-ed').val($('#ed-time').val());
}
function createBrief(data) {
    var s = '';
    s += "<div class='the-brief'>";
    s += "<h3 class='brief-title'><a href='{% url 'get_post' %}?id="+ String(data['NID']) + "' target='_blank' class='brief-title-inner'>" + data['title'] + "</a></h3>";
    //s += "<div class='brief-comment'>" + data['time'] + " - //" + data['source'] + "</div>";
    s += "<p class='brief-content'>" + data['brief'] + '</p>';
    s += "</div>";
    return s;
}
function updBrief() {
    $('.brief-list').empty();
    var ask_list = {};
    var bg = (page - 1) * page_size;
    var ed = page * page_size;
    if (bg >= post_list.length)
        return;
    if (ed > post_list.length)
        ed = post_list.length;
    for (var i = bg; i < ed; ++i) {
        ask_list['' + (i - bg)] = Number(post_list[i]);
    }
    $.ajax({
        url: "{% url 'get_post_brief' %}",
        data: ask_list,
        success: function(ret) {
            for (var i in ret) {
                $('.brief-list').append(createBrief(ret['' + i]));
            }
            var arr = $('.brief-title-inner, .brief-content');
            for (var x = 0; x < arr.length; ++x) {
                var s = arr[x].innerHTML;
                for (var i in ci) {
                    s = s.split(ci[i]).join("<span class='highlight'>" + ci[i] + "</span>");
                }
                arr[x].innerHTML = s;
            }
        }
    });
}
function updPagination() {
    $('.pagination').empty();
    var blk = Math.floor((page - 1) / page_number);
    var bg = blk * page_number + 1;
    var ed = (blk + 1) * page_number;
    if (page == bg && bg != 1) {
        --bg;
        --ed;
    }
    if (page == ed && ed != page_limit) {
        ++bg;
        ++ed;
    }
    if (ed > page_limit)
        ed = page_limit;
    var s = "";
    if (page != 1)
        s += "<li class='page_prev'><a href='#'>&lt;上一页</a></li>";

    for (var i = bg; i <= ed; ++i) {
        s += "<li id='page_" + i + "' the_id='" + i + "'" + (i == page ? " class='active'" : "") + "><a href='#'>" + i + "</a></li>";
    }

    if (page != page_limit)
        s += "<li class='page_next'><a href='#'>下一页&gt;</a></li>";
    $('.pagination').append(s);

    if (page != 1)
        $('.pagination .page_prev').click(function() {
            page--;
            updPagination();
            updBrief();
        });
    if (page != page_limit)
        $('.pagination .page_next').click(function() {
            page++;
            updPagination();
            updBrief();
        });
    for (var i = bg; i <= ed; ++i) {
        $('#page_' + i).click(function() {
            page = $(this).attr('the_id');
            updPagination();
            updBrief();
        });
    }
}
$(document).ready(function(){
    $('#search_edit').attr('value', '{{ wd }}');
    $.ajax({
        url: "{% url 'search_word' %}",
        data: {'wd': '{{ wd }}'},
        success: function(ret) {
            $('#len').html(ret['len']);
            $('#cost').html(ret['cost']);
            ci = ret['word'];
            post_list = ret['data'];
            page_limit = Math.ceil(post_list.length / page_size);
            page = 1;
            updPagination();
            updBrief();
        }
    });
});
