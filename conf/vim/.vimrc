set tabstop=4
set shiftwidth=4
set expandtab
set softtabstop=4
set cindent
colors Dark

autocmd Filetype make setlocal noexpandtab
autocmd Filetype c setlocal tabstop=2|setlocal shiftwidth=2|setlocal softtabstop=2
autocmd Filetype javascript setlocal tabstop=2|setlocal shiftwidth=2|setlocal softtabstop=2

autocmd BufNewFile *.php exec ":call SetComment()" 

func SetComment()
    call setline(1, "<?php") 
    call append(1, "/**") 
    call append(2, " *") 
    call append(3, " *   作者：王川(wangchuan@tencentwsh.com)")
    call append(4, " *   创建时间：".strftime("%Y-%m-%d %H:%M:%S")) 
    call append(5, " *") 
    call append(6, " */")
endfunc


