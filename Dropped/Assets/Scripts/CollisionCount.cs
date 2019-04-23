using UnityEngine;
using UnityEngine.UI;

public class CollisionCount : MonoBehaviour
{
    public GameObject player;
    public Text livesText;
    public Text scoreText;

    // Update is called once per frame
    void Update()
    {

        //livesText.text = "Hello";
        //player.collisionCount + "/5";

        PlayerMove playMove = player.GetComponent<PlayerMove>();
        livesText.text = (5 - playMove.collisionCount).ToString() + " lives remaining! ";
        scoreText.text = "Score: " + playMove.score.ToString();
    }


}
