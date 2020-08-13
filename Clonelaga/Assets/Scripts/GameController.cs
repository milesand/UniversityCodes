using System.Linq;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Events;

public class GameController : MonoBehaviour {

    public float waveCooldown;

    public Text scoreText;
    public Text startLabel;
    public Text levelText;
    public Text gameOverLabel;
    public Text gameOverLabel2;

    public AudioClip introMusic;
    public AudioClip displayRankSFX;
    public AudioClip oneUpSFX;

    public Transform ranks;
    public RankDatum[] rankData;

    public GameObject[] fixedPaths;

    public GameObject[] lifeSymbols;
    public Text AdditionalLife;

    public GameObject fighterPrefab;
    public GameObject zakoPrefab;
    public GameObject goeiPrefab;

    private int score = 0;
    private byte level = 0;
    private AudioSource sfx;
    private Transform respawn;
    private FighterController fighter;

    private List<PathContainer> paths;
    private bool maySpawn;
    private List<int> enemyBlocks;
    private byte enemyLeft = 36;
    private byte life = 3;
    private bool gameOver = false;

    private float timeUntilWave = 100.0f;

	void Start() {
        sfx = GameObject.FindWithTag("Audio_SFX").GetComponent<AudioSource>();
        respawn = GameObject.FindWithTag("Respawn").GetComponent<Transform>();
        paths = new List<PathContainer>();
        foreach (GameObject g in fixedPaths) {
            paths.Add(g.GetComponent<PathContainer>());
        }

        StartCoroutine(Setup());
    }

    private IEnumerator Setup() {
        yield return Intro();
        yield return NextLevel();
        this.fighter = Instantiate(fighterPrefab, respawn).GetComponent<FighterController>();
    }

    private IEnumerator Intro() {
        sfx.PlayOneShot(introMusic);
        startLabel.enabled = true;
        yield return new WaitForSeconds(introMusic.length);
        startLabel.enabled = false;
    }

    private IEnumerator NextLevel() {
        maySpawn = false;
        enemyBlocks = Enumerable.Range(0, 9).ToList<int>();
        enemyLeft = 36;

        level += 1;
        levelText.text = "Level " + level.ToString();
        levelText.enabled = true;

        var children = new List<GameObject>();
        foreach (Transform child in ranks) {
            children.Add(child.gameObject);
        }
        children.ForEach(child => Destroy(child));

        int temp = level;
        Vector3 location = new Vector3(0.0f, 0.0f, 0.0f);
        foreach (RankDatum rd in rankData) {
            Sprite sprite = rd.rankPrefab.GetComponent<SpriteRenderer>().sprite;
            while (temp >= rd.level) {
                temp -= rd.level;
                Instantiate(rd.rankPrefab, ranks.position + location, Quaternion.identity, ranks);
                location += new Vector3((sprite.rect.width + 1.0f) / sprite.pixelsPerUnit, 0.0f, 0.0f);
                sfx.PlayOneShot(displayRankSFX);
                yield return new WaitForSeconds(displayRankSFX.length);
            }
        }
        yield return new WaitForSeconds(0.75f);
        levelText.enabled = false;
        timeUntilWave = waveCooldown / 3.0f;
        maySpawn = true;
    }

    void Update() {
        if (!gameOver) {
            if (maySpawn) {
                timeUntilWave -= Time.deltaTime;
                if (timeUntilWave < 0.0f) {
                    timeUntilWave = waveCooldown;
                    StartCoroutine(SpawnWave());
                }
            }
            if (enemyLeft == 0) {
                StartCoroutine(NextLevel());
            }
        } else {
            if (Input.GetKeyDown(KeyCode.Return)) {
                this.GetComponent<LoadSceneByIndex>().LoadScene(0);
            }
        }
    }

    private IEnumerator SpawnWave() {
        int block1 = GetBlockToSpawn();
        int block2 = GetBlockToSpawn();
        if (block2 != -1) {
            switch (Random.Range(0, 3)) {
                case 0:
                    yield return SpawnWave_0_2(block1, block2);
                    break;
                case 1:
                    yield return SpawnWave_1_2(block1, block2);
                    break;
                case 2:
                    yield return SpawnWave_2_2(block1, block2);
                    break;
                default:
                    break;
            }
        } else if (block1 != -1) {
            yield return SpawnWave_0_1(block1, Random.Range(0, 4));
            maySpawn = false;
        } else {
            maySpawn = false;
        }
    }

    private IEnumerator SpawnWave_0_1(int block, int path_idx) {
        GameObject p = BlockToPrefab(block);
        int[] ids = BlockToId(block);
        for (int i = 0; i < 4; i += 1) {
            SpawnEnemy(p, paths[path_idx], ids[i]);
            yield return new WaitForSeconds(0.05f);
        }
    }

    private IEnumerator SpawnWave_0_2(int block1, int block2) {
        GameObject p1 = BlockToPrefab(block1);
        GameObject p2 = BlockToPrefab(block2);
        int[] ids1 = BlockToId(block1);
        int[] ids2 = BlockToId(block2);
        for (int i = 0; i < 4; i += 1) {
            SpawnEnemy(p1, paths[0], ids1[i]);
            SpawnEnemy(p2, paths[1], ids2[i]);
            yield return new WaitForSeconds(0.05f);
        }
    }

    private IEnumerator SpawnWave_1_2(int block1, int block2) {
        GameObject p1 = BlockToPrefab(block1);
        GameObject p2 = BlockToPrefab(block2);
        int[] ids1 = BlockToId(block1);
        int[] ids2 = BlockToId(block2);
        for (int i = 0; i < 4; i += 1) {
            SpawnEnemy(p1, paths[2], ids1[i]);
            yield return new WaitForSeconds(0.05f);
        }
        for(int i = 0; i < 4; i += 1) {
            SpawnEnemy(p2, paths[2], ids2[i]);
            yield return new WaitForSeconds(0.05f);
        }
    }

    private IEnumerator SpawnWave_2_2(int block1, int block2) {
        GameObject p1 = BlockToPrefab(block1);
        GameObject p2 = BlockToPrefab(block2);
        int[] ids1 = BlockToId(block1);
        int[] ids2 = BlockToId(block2);
        for (int i = 0; i < 4; i += 1) {
            SpawnEnemy(p1, paths[3], ids1[i]);
            yield return new WaitForSeconds(0.05f);
        }
        for(int i = 0; i < 4; i += 1) {
            SpawnEnemy(p2, paths[3], ids2[i]);
            yield return new WaitForSeconds(0.05f);
        }
    }

    private GameObject SpawnEnemy(GameObject prefab, PathContainer path, int id) {
        GameObject enemy = Instantiate(prefab);
        var ec = enemy.GetComponent<EnemyController>();
        ec.Spawn(this.level,id, path);
        return enemy;
    }

    // -1 if No blocks left
    private int GetBlockToSpawn() {
        if (enemyBlocks.Count == 0) {
            return -1;
        }
        int idx = Random.Range(0, enemyBlocks.Count - 1);
        int block = enemyBlocks[idx];
        enemyBlocks.RemoveAt(idx);
        return block;
    }

    private int[] BlockToId(int block) {
        if (block < 5) {
            return new int[4] { block * 2, block * 2 + 1, block * 2 + 10, block * 2 + 11 };
        } else {
            return new int[4] { block * 2 + 10, block * 2 + 11, block * 2 + 18, block * 2 + 19 };
        }
    }

    private GameObject BlockToPrefab(int block) {
        if (block < 5) {
            return zakoPrefab;
        } else {
            return goeiPrefab;
        }
    }

    public void FighterDied() {
        if (life != 0) {
            loseLife();
            StartCoroutine(RespawnFighter());
        } else {
            gameOverLabel.enabled = true;
            gameOverLabel2.enabled = true;
            gameOver = true;
        }
    }

    private void gainLife() {
        sfx.PlayOneShot(oneUpSFX);
        if (life < lifeSymbols.Length) {
            lifeSymbols[life].SetActive(true);
        } else {
            AdditionalLife.text = "+" + (life + 1 - lifeSymbols.Length).ToString();
            AdditionalLife.enabled = true;
        }
        life += 1;
    }

    private void loseLife() {
        life -= 1;
        if (life < lifeSymbols.Length) {
            lifeSymbols[life].SetActive(false);
        } else if (life == lifeSymbols.Length) {
            AdditionalLife.enabled = false;
        } else {
            AdditionalLife.text = "+" + (life - lifeSymbols.Length).ToString();
        }
    }

    private IEnumerator RespawnFighter() {
        yield return new WaitForSeconds(4.0f);
        fighter.transform.position = respawn.position;
        fighter.Activate();
    }

    public void EnemyDiedWithScore(int scoreAcquired) {
        enemyLeft -= 1;
        int before = score / 10000;
        score += scoreAcquired;
        if (score / 10000 != before) {
            gainLife();
        }
        scoreText.text = this.score.ToString();
    }
}

[System.Serializable]
public struct RankDatum {
    public byte level;
    public GameObject rankPrefab;
}

