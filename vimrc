" VIM Configuration File
" Description: Optimized for C/C++ development, but useful also for other things.
" Author: Jagdish Prajapati
"

" set UTF-8 encoding
set enc=utf-8
set fenc=utf-8
set termencoding=utf-8
" disable vi compatibility (emulation of old bugs)
set nocompatible
" use indentation of previous line
set autoindent
" use intelligent indentation for C
set smartindent
" configure tabwidth and insert spaces instead of tabs
set tabstop=4        " tab width is 4 spaces
set shiftwidth=4     " indent also with 4 spaces
set expandtab        " expand tabs to spaces
" wrap lines at 120 chars. 80 is somewaht antiquated with nowadays displays.
set textwidth=120
" turn syntax highlighting on
set t_Co=256
syntax on
"colorscheme wombat256
" turn line numbers on
"set number
" highlight matching braces
set showmatch
" intelligent comments
set comments=sl:/*,mb:\ *,elx:\ */

" Install OmniCppComplete like described on http://vim.wikia.com/wiki/C++_code_completion
" This offers intelligent C++ completion when typing ‘.’ ‘->’ or <C-o>
" Load standard tag files
set tags+=~/.vim/tags/cpp
set tags+=~/.vim/tags/gl
set tags+=~/.vim/tags/sdl
set tags+=~/.vim/tags/qt4

filetype plugin indent on                                                       
"syntax on

"====[ DoxygenToolKit ]==================================================        
" Install DoxygenToolkit from http://www.vim.org/scripts/script.php?script_id=987
let g:DoxygenToolkit_briefTag_pre="@brief \<enter>* @par Operations contract:"
" let g:DoxygenToolkit_briefTag_post="\<enter>*******************************************************************************"
let g:DoxygenToolkit_paramTag_pre="@param[in] "                                   
let g:DoxygenToolkit_returnTag="@return \<enter>*  - "
"let g:DoxygenToolkit_briefTag_funcName="no"                                     
 let g:DoxygenToolkit_startCommentTag = "/**"                                    
 let g:DoxygenToolkit_startCommentBlock = "/*"                                   
"let g:DoxygenToolkit_blockHeader="*******************************************************************************"
"let g:DoxygenToolkit_blockFooter="*******************************************************************************"
"let g:DoxygenToolkit_authorName="Jagdish Prajapati <jagdish.prajapati@lnttechservices.com>"
let g:DoxygenToolkit_authorName="Jagdish Prajapati(ee.jprajapati@ficosa.com)"   
let g:DoxygenToolkit_licenseTag="Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS"
let g:DoxygenToolkit_interCommentTag = "* "

" Enhanced keyboard mappings
"
" in normal mode F2 will save the file
nmap <F2> :w<CR>
" in insert mode F2 will exit insert, save, enters insert again
imap <F2> <ESC>:w<CR>i
" switch between header/source with F4
map <F4> :e %:p:s,.h$,.X123X,:s,.c$,.h,:s,.X123X$,.c,<CR>
" recreate tags file with F5
map <F5> :!ctags -R –c++-kinds=+p –fields=+iaS –extra=+q .<CR>
" create doxygen comment
map <F6> :Dox<CR>
" build using makeprg with <F7>
map <F7> :make<CR>
" build using makeprg with <S-F7>
map <S-F7> :make clean all<CR>
" goto definition with F12
map <F12> <C-]>
" in diff mode we use the spell check keys for merging
"if &diff
"  ” diff settings
"  map <M-Down> ]c
"  map <M-Up> [c
"  map <M-Left> do
"  map <M-Right> dp
"  map <F9> :new<CR>:read !svn diff<CR>:set syntax=diff buftype=nofile<CR>gg
"else
  " spell settings
"  :setlocal spell spelllang=en
"  " set the spellfile - folders must exist
"  set spellfile=~/.vim/spellfile.add
"  map <M-Down> ]s
"  map <M-Up> [s
"endif

set spellfile=~/.vim/spellfile.add
autocmd InsertEnter * setlocal spell spelllang=en
autocmd InsertLeave * setlocal nospell

" +++++++++++ Added by Jagdish ++++++++++++++++++++
syntax enable           " enable syntax processing

"set showcmd             " show command in bottom bar

set cursorline          " highlight current line

"filetype indent on

set wildmenu            " visual autocomplete for command menu

"set lazyredraw          " redraw only when we need to.

set incsearch           " search as characters are entered
set hlsearch            " highlight matches
" turn off search highlight
nnoremap <leader><space> :nohlsearch<CR>

set foldenable          " enable folding
set foldlevelstart=10   " open most folds by default
set foldnestmax=4      " 10 nested fold max
" space open/closes folds
nnoremap <space> za

set foldmethod=indent   " fold based on indent level
"set foldmethod=marker   " fold based on marker level
"set foldmethod=manual   " fold based on manual level
"set foldmethod=expr   " fold based on expression level
"set foldmethod=syntax   " fold based on syntax level
"set foldmethod=diff   " fold based on difference level
"set modelines=1
" modline at the end of file " vim:foldmethod=marker:foldlevel=0

" move vertically by visual line
nnoremap j gj
nnoremap k gk

" move to beginning/end of line
nnoremap B ^
nnoremap E $

" $/^ doesn't do anything
nnoremap $ <nop>
nnoremap ^ <nop>

" highlight last inserted text
"nnoremap gV `[v`]

"let mapleader=","       " leader is comma

" toggle graphical undo (gundo)
nnoremap <leader>u :GundoToggle<CR>

" edit vimrc and load vimrc bindings
"nnoremap <leader>ev :vsp $MYVIMRC<CR>
"nnoremap <leader>sv :source $MYVIMRC<CR>

" save session
"nnoremap <leader>s :mksession<CR>

" open ag.vim (search in whole project without closing current file)
"nnoremap <leader>a :Ag

" Strips trailing whitespace at the end of files. this
" is called on buffer write in the autogroup above.
function! <SID>StripTrailingWhitespaces()
    " save last search & cursor position
    let _s=@/
    let l = line(".")
    let c = col(".")
    %s/\s\+$//e
    let @/=_s
    call cursor(l, c)
endfunction

autocmd BufWritePre *.php,*.py,*.js,*.txt,*.hs,*.java,*.c,*.h,*.cpp,*.md :call <SID>StripTrailingWhitespaces()

" toggle between number and relativenumber
function! ToggleNumber()
    if(&relativenumber == 1)
        set norelativenumber
"        set number
        else
        set relativenumber
    endif
endfunc

nmap <F3> :call ToggleNumber()<CR>
imap <F3> <ESC>:call ToggleNumber()<CR>i

" toggle between paste and nopaste
let &t_SI .= "\<Esc>[?2004h"
let &t_EI .= "\<Esc>[?2004l"

inoremap <special> <expr> <Esc>[200~ XTermPasteBegin()

function! XTermPasteBegin()
   set pastetoggle=<Esc>[201~
   set paste
   return ""
endfunction

set colorcolumn=81
highlight ColorColumn ctermbg=8

