var info, script = {};

(function(){
  $.when($.getJSON('testscript.json')).done(function(data){
    info = data.info;
    script = data.script;
    $('#script_title').html('<b>'+info.title+'</b> by'+info.name);
    $('#script_prev').html('(前のシーンが表示されます)').addClass('script0');
    viewScript('#script_now', 0);
    $('#script_now').addClass('script1');
    viewScript('#script_next', 1);
    $('#script_next').addClass('script2').attr('onclick', 'goNext();');
    $('#script_number').html('1/' + script.length);

    $('#count_start').click(function(){
      starttime = new Date();
      timer = setInterval(function(){
        var currenttime = new Date();
        var passtime = Math.round((currenttime.getTime() - starttime.getTime()) / 1000);
        var hour     = toDoubleDigits(Math.floor(passtime / 3600));
        var minute   = toDoubleDigits(Math.floor((passtime - 3600*hour) / 60));
        var second   = toDoubleDigits((passtime - 3600*hour - 60*minute));
        $('#script_timecount').html(hour + ':' + minute + ':' + second);
        return;
      }, 1000);
    });
    $('#count_stop').click(function(){clearInterval(timer)});
  });
}());

function toDoubleDigits(num){return ('0' + num).slice(-2)}; // sliceで時刻要素の0埋め

function goNext(){
  $.each(['script_prev', 'script_now', 'script_next'], function(){
    var num = $('#'+this).get(0).className.match(/\d/g).join('') / 1; // 数字だけ取り出して渡す(型変換しないとうまくいかなかった)
    $('#'+this).removeClass($('#'+this).get(0).className).addClass('script' + (num+1));
    if(num+1 > script.length) $('#'+this).html('(原稿の最後です)').removeAttr('onclick');
    else{
      if(this == 'script_now') sendComm(num, 0);
      viewScript('#'+this, num);
    }
  });
  $('#script_number').html($('#script_now').get(0).className.match(/\d/g).join('') + '/' + script.length);
}

function goPrev(){
  $.each(['script_prev', 'script_now', 'script_next'], function(){
    var num = $('#'+this).get(0).className.match(/\d/g).join('') / 1; // 数字だけ取り出して渡す(型変換しないとうまくいかなかった)
    $('#'+this).removeClass($('#'+this).get(0).className).addClass('script' + (num-1));
    if(num-1 <= 0) $('#'+this).html('(前のシーンが表示されます)').removeAttr('onclick');
    else{
      if(this == 'script_now') sendComm(num-1, 1);
      viewScript('#'+this, num-2);
    }
  });
  $('#script_number').html($('#script_now').get(0).className.match(/\d/g).join('') + '/' + script.length);
}

function sendComm(index, reverse){
  var data = $.extend(true, {}, script[index].projector);
  if(reverse) $.each(data, function(key){
    data[key] = this == 1 ? 0 : 1;
    });
  $.each(ip, function(){
    address = this + 'setPort/status.json';
    getRequest(address, data).done(function(res){checkStatus(res)});
  });
}

function viewScript(id, index){
  if(!$(id).attr('onclick') && id==='#script_prev') $(id).attr('onclick', 'goPrev();');
  else if(!$(id).attr('onclick') && id==='#script_next') $(id).attr('onclick', 'goNext();');
  $(id).html(function(){
    var res = '<b>「'+script[index].word+'」</b>の';
    switch(script[index].timing){
      case 'pre': res += '前'; break;
      case 'post': res += '後'; break;
      default: res +=  script[index].timing; break;
    }
    $.each(script[index].projector, function(key){
      res += '<br>' + port[key].name + 'を' + (this == 1 ? '点灯' : '消灯');
    });
    return res;
  });
}

