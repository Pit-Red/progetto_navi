import java.io.*; 
import java.util.*;
import static java.lang.System.out;

public class Lexer {

    public static int line = 1;
    private char peek = ' ';
    private char comment_type = ' ';
    
    private void readch(BufferedReader br) {
        try {
            peek = (char) br.read();
        } catch (IOException exc) {
            peek = (char) -1; // ERROR
        }
    }

    private Token isWord(String s){
        switch(s){
            case("assign"):
                return Word.assign;
            case("to"):
                return Word.to;
            case("conditional"):
                return Word.conditional;
            case("option"):
                return Word.option;
            case("do"):
                return Word.dotok;
            case("else"):
                return Word.elsetok;
            case("while"):
                return Word.whiletok;
            case("begin"):
                return Word.begin;
            case("end"):
                return Word.end;
            case("print"):
                return Word.print;
            case("read"):
                return Word.read;
        }
        return null;
    }

    private void markbr(BufferedReader br){
        try {
            br.mark(3);
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }  
    }

    private void resetbr(BufferedReader br){
        try {
            br.reset();
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }  
    }

    private boolean opencomment(BufferedReader br){
        markbr(br);
        if(peek=='/'){
            readch(br);
            if(peek=='*'){
                readch(br);
                comment_type = 'c';
                return true;
            }
        }
        resetbr(br);
        return false;
    }

    private boolean closecomment(BufferedReader br){
        markbr(br);
        if(peek=='*'){
            readch(br);
            if(peek=='/'){
                readch(br);
                comment_type = ' ';
                return true;
            }
        }
        resetbr(br);
        return false;
    }

    private boolean doubleslash(BufferedReader br){
        markbr(br);
        if(peek=='/'){
            readch(br);
            if(peek=='/'){
                readch(br);
                comment_type = 'd';    
                return true;
            }
        }
        resetbr(br);
        return false;
    }

    public Token lexical_scan(BufferedReader br) {
        while (peek == ' ' || peek == '\t' || peek == '\n'  || peek == '\r'||opencomment(br)||doubleslash(br) ) {
            if(comment_type=='c'||comment_type=='d'){ 
                switch(comment_type){
                    case 'c':
                        while(!closecomment(br)){
                            readch(br);
                        }
                        break;
                    case 'd':
                        while(peek!='\n'){
                            readch(br);
                        }
                        comment_type = ' ';
                        break;
                }
            }
            else
                readch(br);
            if (peek == '\n') line++;
        }
       
            //System.out.println("sono dentro al while");
    
        switch (peek) {
            case '/':
                peek = ' ';
                return Token.div;
            case '!':
                peek = ' ';
                return Token.not;

	// ... gestire i casi di ( ) [ ] { } + - * / ; , ... //
            case '(':
                peek = ' ';
                return Token.lpt;
            case ')':
                peek = ' ';
                return Token.rpt;
            case '[':
                peek = ' ';
                return Token.lpq;
            case ']':
                peek = ' ';
                return Token.rpq;
            case '{':
                peek = ' ';
                return Token.lpg;
            case '}':
                peek = ' ';
                return Token.rpg;
            case '+':
                peek = ' ';
                return Token.plus;
            case '-':
                peek = ' ';
                return Token.minus;
            case '*':
                peek = ' ';
                return Token.mult;
            case ';':
                peek = ' ';
                return Token.semicolon;
            case ',':
                peek = ' ';
                return Token.comma;
	
            case '&':
                readch(br);
                if (peek == '&') {
                    peek = ' ';
                    return Word.and;
                } else {
                    System.err.println("Erroneous character"
                            + " after & : "  + peek );
                    return null;
                }

	// ... gestire i casi di || < > <= >= == <> ... //

            case '|':
                readch(br);
                if (peek == '|') {
                    peek = ' ';
                    return Word.or;
                } else {
                    System.err.println("Erroneous character"
                            + " after | : "  + peek );
                    return null;
                }
            case '<':
                markbr(br);
                readch(br);
                if (peek == '=') {
                    peek = ' ';
                    return Word.le;
                } else if(peek == '>') {
                    peek = ' ';
                    return Word.ne;
                } else {
                    peek = ' ';
                    resetbr(br);
                    return Word.lt;
                }
            case '>':
                markbr(br);              
                readch(br);
                if (peek == '=') {
                    peek = ' ';
                    return Word.ge;
                } else {
                    peek = ' ';
                    resetbr(br);
                    return Word.gt;    
                }
                
            case '=':
                readch(br);
                if (peek == '=') {
                    peek = ' ';
                    return Word.eq;
                } else {
                    System.err.println("Erroneous character"
                            + " after = : "  + peek );
                    return null;
                }
            

                  
            case (char)-1:
                return new Token(Tag.EOF);

            default:
                if (Character.isLetter(peek)||peek=='_') {

	// ... gestire il caso degli identificatori e delle parole chiave //
                    String s = "";
                    while(Character.isLetter(peek)||peek=='_'||Character.isDigit(peek)){
                        s+=peek;
                        markbr(br);
                        readch(br);
                        if(!(Character.isLetter(peek)||peek=='_'||Character.isDigit(peek))){
                            if(s.equals("_")){
                                System.err.println("Incorrect declaration");
                                return null;
                            }
                            resetbr(br);
                            Token temp  = isWord(s);
                            if(temp!=null){
                                peek = ' ';
                                return temp;
                            }
                            else{
                                peek = ' ';
                                return new Word(257, s);
                            }
                        }
                    }
                    System.err.println("Erroneous character ciao");
                    return null;

                } else if (Character.isDigit(peek)) {

	// ... gestire il caso dei numeri ... //
                    String num = "";
                    while(Character.isDigit(peek)){
                        num+=peek;
                        markbr(br);
                        readch(br);
                        if(!Character.isDigit(peek)){
                            resetbr(br);
                            peek = ' ';
                            return new NumberTok(256, num);
                        }
                    }
                    
                    
                    return null;
                } else {
                        System.err.println("Erroneous character: fine" 
                                + peek );
                        return null;
                }
         }
    }
		
    public static void main(String[] args) {
        Lexer lex = new Lexer();
        String path = "/home/pit/Scrivania/prog/LFT/es 2/dio"; // il percorso del file da leggere
        try {            
            BufferedReader br = new BufferedReader(new FileReader(path));
            Token tok;
            System.out.println(br.markSupported());
            do {
                tok = lex.lexical_scan(br);
                System.out.println("Scan: " + tok);
            } while (tok.tag != Tag.EOF);
            br.close();
        } catch (IOException e) {e.printStackTrace();}    
    }

}
