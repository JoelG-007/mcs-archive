import scala.io.StdIn
object VowelsConsonants{
  def main(args: Array[String]): Unit ={
    print("Enter a string: ")
    val str = StdIn.readLine().toLowerCase()

    var vowels = 0
    var consonants = 0

    for(ch <- str){
      if(ch.isLetter){
        if("aeiou".contains(ch))
          vowels += 1
        else
          consonants += 1
      }
    }
    println("Vowels = " + vowels)
    println("Consonants = " + consonants)
  }
}